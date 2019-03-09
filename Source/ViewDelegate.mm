//  Copyright © 2019 Whelan Callahan. All rights reserved.

#import "ViewDelegate.h"
#import "ShaderTypes.h"
#import "Math/MatrixOperations.hpp"
#include "Mesh/CPUMesh.hpp"
#include "Mesh/MeshLoader.hpp"
#include "Mesh/GPUMesh.hpp"


@implementation ViewDelegate {
    id<MTLDevice> mDevice;
    id<MTLCommandQueue> mCmdQ;
    id<MTLRenderPipelineState> mPipeline;
    id<MTLDepthStencilState> mDepthStencilState;
    float mScreenWidth;
    float mScreenHeight;
    // temp
    id<MTLBuffer> mUniformBuffer;
    std::vector<GPUMesh> mMeshes;
}


- (instancetype)initWithMtkView: (MTKView *)view
{
    if (self = [super init])
    {
        mDevice = MTLCreateSystemDefaultDevice();
        if (!mDevice) {
            NSLog(@"Could not create default system device");
            return nil;
        }
        view.device = mDevice;
        
        mCmdQ = [mDevice newCommandQueue];
        if (!mCmdQ) {
            NSLog(@"Could not create command queue");
            return nil;
        }
        
        [self _buildPipelineWithView:view];
        
        // temp
        [self _buildBuffers];
    }
    
    return self;
}


- (void)_buildBuffers
{
    //std::vector<std::shared_ptr<CPUMesh>> meshDatas = MeshLoader::LoadFromFile("../../../sword.fbx");
    std::vector<std::shared_ptr<CPUMesh>> meshDatas = MeshLoader::LoadFromFile("../../../BaseMesh_Anim.fbx");
    
    if (meshDatas.size()) {
        std::shared_ptr<CPUMesh> data = meshDatas[0];
        
        GPUMesh mesh;
        
        id<MTLCommandBuffer> commandBuffer = [mCmdQ commandBuffer];
        id<MTLBlitCommandEncoder> encoder = [commandBuffer blitCommandEncoder];
        id<MTLBuffer> vertexUpload;
        id<MTLBuffer> indexUpload;
        
        mesh.vertices = [self _createPrivateBufferWithBytes:&(data->vertices[0])
                                                     length:sizeof(Vertex) * data->vertices.size()
                                                blitEncoder:encoder
                                               uploadBuffer:&vertexUpload];
        
        mesh.indices = [self _createPrivateBufferWithBytes:&(data->indices[0])
                                                    length:sizeof(Index) * data->indices.size()
                                               blitEncoder:encoder
                                              uploadBuffer:&indexUpload];
        
        mesh.indexCount = data->indices.size();
        mesh.uniformOffset = 0;
        mMeshes.push_back(mesh);
        
        mUniformBuffer = [mDevice newBufferWithLength:sizeof(Uniform) options:MTLResourceStorageModeShared];
        
        [encoder endEncoding];
        [commandBuffer commit];
    }
}


- (id<MTLBuffer>)_createPrivateBufferWithBytes:(const void *)data
                                        length:(NSUInteger)length
                                   blitEncoder:(id<MTLBlitCommandEncoder>)encoder
                                  uploadBuffer:(id<MTLBuffer>*)outUpload
{
    id<MTLBuffer> upload = [mDevice newBufferWithBytes:data
                                                length:length
                                               options:MTLResourceStorageModeShared];
    
    id<MTLBuffer> pBuff = [mDevice newBufferWithLength:length options:MTLResourceStorageModePrivate];
    
    [encoder copyFromBuffer:upload sourceOffset:0 toBuffer:pBuff destinationOffset:0 size:length];
    
    *outUpload = upload;
    return pBuff;
}


- (void)_buildPipelineWithView: (nonnull MTKView *)view
{ 
    NSError *error = NULL;
    MTLRenderPipelineDescriptor *descriptor = [[MTLRenderPipelineDescriptor alloc] init];
    
    id<MTLLibrary> library = [mDevice newDefaultLibrary];
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
    
    mPipeline = [mDevice newRenderPipelineStateWithDescriptor:descriptor error:&error];
    if (!mPipeline || error) {
        NSLog(@"Could not create pipeline: %@", error.classDescription);
        return;
    }
    
    
    MTLDepthStencilDescriptor *depthStencilDescriptor = [MTLDepthStencilDescriptor new];
    depthStencilDescriptor.depthCompareFunction = MTLCompareFunctionLess;
    depthStencilDescriptor.depthWriteEnabled = YES;
    mDepthStencilState = [mDevice newDepthStencilStateWithDescriptor:depthStencilDescriptor];
}


- (void) _drawMesh: (GPUMesh *)mesh withEncoder: (id<MTLRenderCommandEncoder>)encoder
{
    [encoder setVertexBuffer:mesh->vertices offset:0 atIndex:0];
    [encoder setVertexBuffer:mUniformBuffer offset:sizeof(Uniform)*mesh->uniformOffset atIndex:1];
    [encoder drawIndexedPrimitives:MTLPrimitiveTypeTriangle
                        indexCount:mesh->indexCount
                         indexType:MTLIndexTypeUInt32
                       indexBuffer:mesh->indices
                 indexBufferOffset:0];
}


- (void)drawInMTKView: (nonnull MTKView *)view
{
    id<MTLCommandBuffer> cmdBuff = [mCmdQ commandBuffer];
    
    MTLRenderPassDescriptor *renderPassDescriptor = view.currentRenderPassDescriptor;
    renderPassDescriptor.colorAttachments[0].clearColor = MTLClearColorMake(63.0/255, 133.0/255, 193.0/255, 1.0);
    id<MTLRenderCommandEncoder> encoder = [cmdBuff renderCommandEncoderWithDescriptor:renderPassDescriptor];
    
    // temp set uniform
    static float degree = 0.0f;
    degree += 1.0f;
    matrix_float4x4 modelScale = ScaleMatrix(0.1f);
    matrix_float4x4 modelUpright = RotationMatrix(vector3(1.0f, 0.0f, 0.0f), -90.0f * (3.14159 / 180.0));
    matrix_float4x4 modelRotation = matrix_multiply(RotationMatrix(vector3(0.0f, 1.0f, 0.0f), degree * (3.14159 / 180.0)), modelUpright);
    matrix_float4x4 modelTranslation = TranslationMatrix(0, 0, -30);
    matrix_float4x4 modelMatrix = matrix_multiply(matrix_multiply(modelTranslation, modelRotation), modelScale);
    matrix_float4x4 projMatrix = PerspectiveMatrixRH(60.0f*(3.14159/180.0f), mScreenWidth/mScreenHeight, 0.1f, 1000.0f);
    
    Uniform uniform;
    uniform.projViewModelMatrix = matrix_multiply(projMatrix, modelMatrix);
    uniform.normalMatrix = NormalMatrix(modelMatrix);
    memcpy(mUniformBuffer.contents, &uniform, sizeof(Uniform));
    
    [encoder setDepthStencilState:mDepthStencilState];
    [encoder setRenderPipelineState:mPipeline];
    
    for (GPUMesh &mesh : mMeshes) {
        [self _drawMesh:&mesh withEncoder:encoder];
    }
    
    [encoder endEncoding];
    [cmdBuff presentDrawable:view.currentDrawable];
    [cmdBuff commit];
}


- (void)mtkView: (nonnull MTKView *)view drawableSizeWillChange: (CGSize)size
{
    mScreenWidth = size.width;
    mScreenHeight = size.height;
}

@end
