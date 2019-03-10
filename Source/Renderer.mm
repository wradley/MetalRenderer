//  Copyright © 2019 Whelan Callahan. All rights reserved.
#include <MetalKit/MetalKit.h>
#include <iostream>
#include "Renderer.hpp"
#include "Mesh/GPUMesh.hpp"
#include "Mesh/CPUMesh.hpp"
#include "Texture/Texture.hpp"
#include "Math/Math.hpp"
#include "Mesh/MeshLoader.hpp"


id<MTLBuffer> CreatePrivateBufferWithBytes(
    const void *data,
    unsigned int length,
    id<MTLDevice> device,
    id<MTLBlitCommandEncoder> encoder,
    id<MTLBuffer> *outUploadBuffer
){
    id<MTLBuffer> upload = [device newBufferWithBytes:data
                                               length:length
                                              options:MTLResourceStorageModeShared];
    
    id<MTLBuffer> pBuff = [device newBufferWithLength:length
                                              options:MTLResourceStorageModePrivate];
    
    [encoder copyFromBuffer:upload sourceOffset:0 toBuffer:pBuff destinationOffset:0 size:length];
    
    *outUploadBuffer = upload;
    return pBuff;
}


struct Renderer::Data
{
    id<MTLDevice> device;
    id<MTLCommandQueue> cmdQ;
    id<MTLRenderPipelineState> renderPipeline;
    id<MTLDepthStencilState> depthStencilState;
    //float screenWidth;
    //float screenHeight;
    
    id<MTLBuffer> uniformBuffer;
    unsigned int numUniforms;
    unsigned int nextFreeUniform;
};


Renderer::Renderer(id device, id View)
{
    MTKView *view = View;
    mData = new Data;
    mData->device = device;
    mData->cmdQ = [device newCommandQueue];
    if (!mData->cmdQ) {
        NSLog(@"Could not create command queue");
        return;
    }
    
    // render pipeline
    NSError *error = NULL;
    MTLRenderPipelineDescriptor *descriptor = [[MTLRenderPipelineDescriptor alloc] init];
    
    id<MTLLibrary> library = [mData->device newDefaultLibrary];
    if (!library) {
        NSLog(@"Could not create default library");
        return;
    }
    
    id<MTLFunction> vertexFn = [library newFunctionWithName:@"VertexFunction"];
    id<MTLFunction> fragFn = [library newFunctionWithName:@"FragmentFunction"];
    
    descriptor.vertexFunction = vertexFn;
    descriptor.fragmentFunction = fragFn;
    descriptor.colorAttachments[0].pixelFormat = view.colorPixelFormat;
    descriptor.depthAttachmentPixelFormat = view.depthStencilPixelFormat;
    
    mData->renderPipeline = [mData->device newRenderPipelineStateWithDescriptor:descriptor error:&error];
    if (!mData->renderPipeline || error) {
        NSLog(@"Could not create pipeline: %@", error.classDescription);
        return;
    }
    
    // depth stencil
    MTLDepthStencilDescriptor *depthStencilDescriptor = [MTLDepthStencilDescriptor new];
    depthStencilDescriptor.depthCompareFunction = MTLCompareFunctionLess;
    depthStencilDescriptor.depthWriteEnabled = YES;
    mData->depthStencilState = [mData->device newDepthStencilStateWithDescriptor:depthStencilDescriptor];
    
    // uniforms
    mData->numUniforms = 100;
    mData->nextFreeUniform = 0;
    mData->uniformBuffer = [mData->device newBufferWithLength:mData->numUniforms * sizeof(Uniform) options:MTLResourceStorageModeShared];
    
    mGroup = new RenderGroup(nullptr);
    mCamera = nullptr;
}


Renderer::~Renderer()
{
    delete mGroup;
}


RenderGroup* Renderer::getRootGroup()
{
    return mGroup;
}


void Renderer::setCamera(Camera *c)
{
    mCamera = c;
}


void Renderer::draw(id View)
{
    // update uniform data
    matrix_float4x4 viewMatrix = mCamera->getGlobalViewMatrix();
    //matrix_float4x4 viewMatrix = matrix_identity_float4x4;
    matrix_float4x4 projMatrix = Matrix::Perspective(60.0f*(3.14159/180.0f), mCamera->getAspectRatio(), 0.1f, 1000.0f);
    Uniform uniforms[mData->numUniforms];
    updateModelUniforms(simd_mul(projMatrix, viewMatrix), uniforms, mGroup);
    memcpy(mData->uniformBuffer.contents, &uniforms, sizeof(Uniform) * mData->numUniforms);
    
    // draw
    MTKView *view = View;
    id<MTLCommandBuffer> cmdBuff = [mData->cmdQ commandBuffer];
    
    MTLRenderPassDescriptor *renderPassDescriptor = view.currentRenderPassDescriptor;
    renderPassDescriptor.colorAttachments[0].clearColor = MTLClearColorMake(63.0/255, 133.0/255, 193.0/255, 1.0);
    id<MTLRenderCommandEncoder> encoder = [cmdBuff renderCommandEncoderWithDescriptor:renderPassDescriptor];
    
    [encoder setDepthStencilState:mData->depthStencilState];
    [encoder setRenderPipelineState:mData->renderPipeline];
    
    drawModels(mGroup, encoder);
    
    [encoder endEncoding];
    [cmdBuff presentDrawable:view.currentDrawable];
    [cmdBuff commit];
}


std::shared_ptr<GPUMesh> Renderer::createMesh(std::shared_ptr<CPUMesh> cpuMesh)
{
    GPUMesh *mesh = new GPUMesh;
    
    id<MTLCommandBuffer> commandBuffer = [mData->cmdQ commandBuffer];
    id<MTLBlitCommandEncoder> encoder = [commandBuffer blitCommandEncoder];
    id<MTLBuffer> vertexUpload;
    id<MTLBuffer> indexUpload;
    
    mesh->vertices = CreatePrivateBufferWithBytes(&(cpuMesh->vertices[0]),
                                                  sizeof(Vertex)*cpuMesh->vertices.size(),
                                                  mData->device,
                                                  encoder,
                                                  &vertexUpload);
    
    mesh->indices = CreatePrivateBufferWithBytes(&(cpuMesh->indices[0]),
                                                 sizeof(Index)*cpuMesh->indices.size(),
                                                 mData->device,
                                                 encoder,
                                                 &indexUpload);
    
    mesh->indexCount = cpuMesh->indices.size();
    mesh->uniformOffset = mData->nextFreeUniform;
    mData->nextFreeUniform += sizeof(Uniform);
    
    [encoder endEncoding];
    [commandBuffer commit];
    
    return std::shared_ptr<GPUMesh>(mesh);
}


std::shared_ptr<Texture> Renderer::createTexture(const char *fp)
{
    id<MTLCommandBuffer> commandBuffer = [mData->cmdQ commandBuffer];
    id<MTLBlitCommandEncoder> encoder = [commandBuffer blitCommandEncoder];
    
    NSString *NSfp = [[NSString alloc] initWithUTF8String:fp];
    NSURL *textureURL = [[NSURL alloc] initFileURLWithPath:NSfp];
    NSError *error = NULL;
    MTKTextureLoader *textureLoader = [[MTKTextureLoader alloc] initWithDevice:mData->device];
    id<MTLTexture> mtlTexture = [textureLoader newTextureWithContentsOfURL:textureURL options:nil error:&error];
    
    [encoder endEncoding];
    [commandBuffer commit];
    
    auto texture = std::make_shared<Texture>();
    texture->texture = mtlTexture;
    return texture;
}


void Renderer::drawModels(RenderGroup *g, id Encoder)
{
    for (Model *model : g->mModels) {
        id<MTLRenderCommandEncoder> encoder = Encoder;
        [encoder setFragmentTexture:model->mColor->texture atIndex:0];
        [encoder setVertexBuffer:model->mMesh->vertices offset:0 atIndex:0];
        [encoder setVertexBuffer:mData->uniformBuffer offset:sizeof(Uniform)*model->mMesh->uniformOffset atIndex:1];
        [encoder drawIndexedPrimitives:MTLPrimitiveTypeTriangle
                            indexCount:model->mMesh->indexCount
                             indexType:MTLIndexTypeUInt32
                           indexBuffer:model->mMesh->indices
                     indexBufferOffset:0];
    }
    
    for (RenderGroup *child : g->mGroups)
        drawModels(child, Encoder);
}


void Renderer::updateModelUniforms(simd_float4x4 projViewMat, Uniform *uniforms, RenderGroup *g)
{
    for (Model *model : g->mModels) {
        simd_float4x4 modelMatrix = model->getGlobalTransformMatrix();
        uniforms[model->mMesh->uniformOffset].projViewModelMatrix = simd_mul(projViewMat, modelMatrix);
        uniforms[model->mMesh->uniformOffset].normalMatrix = Matrix::Normal(modelMatrix);
    }
    
    for (RenderGroup *child : g->mGroups)
        updateModelUniforms(projViewMat, uniforms, child);
}

