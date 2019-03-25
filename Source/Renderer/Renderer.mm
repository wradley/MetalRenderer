//  Copyright © 2019 Whelan Callahan. All rights reserved.
#include <MetalKit/MetalKit.h>
#include <iostream>
#include "Renderer.hpp"
#include "../Math/Math.hpp"
#include "../ShaderTypes.h"


static MTLVertexDescriptor* GetVertexDescriptor()
{
    MTLVertexDescriptor *vertexDescriptor = [MTLVertexDescriptor vertexDescriptor];
    vertexDescriptor.attributes[0].format = MTLVertexFormatFloat3;      // -- position
    vertexDescriptor.attributes[0].bufferIndex = 0;
    vertexDescriptor.attributes[0].offset = offsetof(Vertex, position);
    
    vertexDescriptor.attributes[1].format = MTLVertexFormatFloat3;      // -- normal
    vertexDescriptor.attributes[1].bufferIndex = 0;
    vertexDescriptor.attributes[1].offset = offsetof(Vertex, normal);
    
    vertexDescriptor.attributes[2].format = MTLVertexFormatFloat2;      // -- uv
    vertexDescriptor.attributes[2].bufferIndex = 0;
    vertexDescriptor.attributes[2].offset = offsetof(Vertex, uv);
    
    vertexDescriptor.attributes[3].format = MTLVertexFormatInt4;        // -- boneIDs
    vertexDescriptor.attributes[3].bufferIndex = 0;
    vertexDescriptor.attributes[3].offset = offsetof(Vertex, boneIDs);
    
    vertexDescriptor.attributes[4].format = MTLVertexFormatFloat4;      // -- boneWeights
    vertexDescriptor.attributes[4].bufferIndex = 0;
    vertexDescriptor.attributes[4].offset = offsetof(Vertex, boneWeights);
    
    
    vertexDescriptor.layouts[0].stride = sizeof(Vertex);
    vertexDescriptor.layouts[0].stepFunction = MTLVertexStepFunctionPerVertex;
    
    return vertexDescriptor;
}


struct Renderer::Data
{
    id<MTLDevice> device;
    id<MTLCommandQueue> cmdQ;
    id<MTLRenderPipelineState> renderPipeline;
    id<MTLDepthStencilState> depthStencilState;
};


Renderer::Renderer(id device, id View) :
    objectDelegate(nullptr),
    resourceDelegate(nullptr)
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

    descriptor.vertexDescriptor = GetVertexDescriptor();
    descriptor.vertexFunction = vertexFn;
    descriptor.fragmentFunction = fragFn;
    descriptor.colorAttachments[0].pixelFormat = view.colorPixelFormat;
    descriptor.depthAttachmentPixelFormat = view.depthStencilPixelFormat;

    mData->renderPipeline = [mData->device newRenderPipelineStateWithDescriptor:descriptor error:&error];
    if (!mData->renderPipeline || error) {
        NSLog(@"Could not create pipeline: %@", error.userInfo);
        return;
    }

    // depth stencil
    MTLDepthStencilDescriptor *depthStencilDescriptor = [MTLDepthStencilDescriptor new];
    depthStencilDescriptor.depthCompareFunction = MTLCompareFunctionLess;
    depthStencilDescriptor.depthWriteEnabled = YES;
    mData->depthStencilState = [mData->device newDepthStencilStateWithDescriptor:depthStencilDescriptor];

    initializeInternals();
}


Renderer::~Renderer()
{
    delete mData;
}


uint64 Renderer::getRootGroup()
{
    return 1;
}


void Renderer::setCamera(uint64 cam)
{
    mCurrCamera = cam;
}


void Renderer::draw(id View, double time)
{
    if (mCurrCamera == NIL_INDEX) {
        std::cout << "Renderer's current camera is not set." << std::endl;
        return;
    }

    updateUniforms();
    updateAnimations(time);

    // initialize draw
    MTKView *view = View;
    id<MTLCommandBuffer> cmdBuff = [mData->cmdQ commandBuffer];

    MTLRenderPassDescriptor *renderPassDescriptor = view.currentRenderPassDescriptor;
    renderPassDescriptor.colorAttachments[0].clearColor = MTLClearColorMake(63.0/255, 133.0/255, 193.0/255, 1.0);
    id<MTLRenderCommandEncoder> encoder = [cmdBuff renderCommandEncoderWithDescriptor:renderPassDescriptor];

    [encoder setFrontFacingWinding:MTLWindingClockwise];
    [encoder setCullMode:MTLCullModeBack];
    [encoder setDepthStencilState:mData->depthStencilState];
    [encoder setRenderPipelineState:mData->renderPipeline];

    // draw models
    drawModels(encoder, time);

    // commit
    [encoder endEncoding];
    [cmdBuff presentDrawable:view.currentDrawable];
    [cmdBuff commit];
}


void Renderer::initializeInternals()
{
    // push nil objects into lists so that next pushed objects are valid
    mGroups.push_back(InternalGroup());
    mGroups.push_back(InternalGroup());
    mModels.push_back(InternalModel());
    mCameras.push_back(InternalCamera());

    mGroups[1].parent = NIL_INDEX;
    mCurrCamera = NIL_INDEX;
    
    objectDelegate = std::make_unique<RendererObjectDelegate>(this);
    resourceDelegate = std::make_unique<RendererResourceDelegate>(mData->device,
                                                                  mData->cmdQ,
                                                                  mVertexIndexBuffers,
                                                                  mUniformBuffers,
                                                                  mBoneBuffers,
                                                                  mTextures);
}


void Renderer::drawModels(id Encoder, double time)
{
    id<MTLRenderCommandEncoder> encoder = Encoder;
    int iteration = 0;
    for (InternalModel &model : mModels)
    {
        if (!iteration++) // skip nil model
            continue;
        
        // vertex
        [encoder setVertexBuffer:mVertexIndexBuffers[model.mesh.vertexIndexBuffer]
                          offset:0
                         atIndex:0];
        [encoder setVertexBuffer:mUniformBuffers[model.mesh.uniformBuffer]
                          offset:model.mesh.uniformBufferOffset
                         atIndex:1];
        
        if (model.skeleton.boneCount)
        {
            [encoder setVertexBuffer:mBoneBuffers[model.mesh.boneBuffer]
                              offset:model.mesh.boneBufferOffset
                             atIndex:2];
        }
        
        else // set dummy buffer
        {
            [encoder setVertexBuffer:mUniformBuffers[model.mesh.uniformBuffer]
                              offset:model.mesh.uniformBufferOffset
                             atIndex:2];
        }
        

        // fragment
        [encoder setFragmentTexture:mTextures[model.colorTexture]
                            atIndex:0];

        // draw
        [encoder drawIndexedPrimitives:MTLPrimitiveTypeTriangle
                            indexCount:model.mesh.indexCount
                             indexType:MTLIndexTypeUInt32
                           indexBuffer:mVertexIndexBuffers[model.mesh.vertexIndexBuffer]
                     indexBufferOffset:model.mesh.indexOffset];
    }
}


void Renderer::updateAnimations(double time)
{
    int iteration = 0;
    for (InternalModel &model : mModels)
    {
        if (!iteration++) // skip nil model
            continue;
        
        if (!model.skeleton.boneCount)
            continue;
        
        if (model.currAnimation >= model.animationCount)
            continue;
        
        id<MTLBuffer> boneMats = mBoneBuffers[model.mesh.boneBuffer];
        void *boneMatsDst = (char*)boneMats.contents + model.mesh.boneBufferOffset;
        model.calculateBoneMatrices(boneMatsDst, time);
    }
}


void Renderer::updateUniforms()
{
    // get camera data
    InternalCamera &cam = mCameras[mCurrCamera];
    simd_float4x4 camMat = calcWorldMatrix(mGroups[cam.parent]);
    simd_float4 camPos = simd_mul(camMat, simd_make_float4(0.0f, 0.0f, 0.0f, 1.0f));
    simd_float4 camTarget = simd_mul(camMat, simd_make_float4(0.0f, 0.0f, 1.0f, 1.0f));

    // calculate view and proj matrices
    simd_float4x4 proj = Matrix::Perspective(cam.fieldOfView, cam.aspectRatio, cam.nearPlane, cam.farPlane);
    simd_float4x4 view = Matrix::LookAtRH(simd_make_float3(camPos), simd_make_float3(camTarget), simd_make_float3(0.0f, 1.0f, 0.0f));
    simd_float4x4 projView = simd_mul(proj, view);

    // calculate per model data
    int iteration = 0;
    for (InternalModel &model : mModels)
    {
        if (!iteration++) // skip nil model
            continue;
        
        matrix_float4x4 modelMat = calcWorldMatrix(mGroups[model.parent]);

        id<MTLBuffer> uniformBuffer = mUniformBuffers[model.mesh.uniformBuffer];
        void *uniformDst = (char*)uniformBuffer.contents + model.mesh.uniformBufferOffset;
        Uniform uniform;
        uniform.projViewWorldMatrix = simd_mul(projView, modelMat);
        uniform.normalMatrix = Matrix::Normal(simd_mul(view, modelMat));
        uniform.boneCount = model.skeleton.boneCount;
        memcpy(uniformDst, &uniform, sizeof(Uniform));
    }
}


simd_float4x4 Renderer::calcWorldMatrix(const InternalGroup &g) const
{
    if (g.parent == NIL_INDEX) return g.transform.getMatrix();
    else {
        matrix_float4x4 parent = calcWorldMatrix(mGroups[g.parent]);
        return matrix_multiply(parent, g.transform.getMatrix());
    }
}
