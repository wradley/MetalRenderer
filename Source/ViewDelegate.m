//  Copyright © 2019 Whelan Callahan. All rights reserved.

#import "ViewDelegate.h"
#import "ShaderTypes.h"
#import "Math/MatrixOperations.h"

Vertex verts[] = {
    {{1.0f, 0.0f, 0.0f, 1.0f}, {-0.5f, -0.5f, 0.0f}},
    {{1.0f, 0.0f, 1.0f, 1.0f}, { 0.5f, -0.5f, 0.0f}},
    {{0.0f, 1.0f, 0.0f, 1.0f}, {-0.5f,  0.5f, 0.0f}},
    {{0.0f, 1.0f, 1.0f, 1.0f}, { 0.5f,  0.5f, 0.0f}}
};

typedef uint16 Index;
Index indices[] = {
    0, 2, 3,    3, 1, 0
};

@implementation ViewDelegate {
    id<MTLDevice> mDevice;
    id<MTLCommandQueue> mCmdQ;
    id<MTLRenderPipelineState> mPipeline;
    
    // temp
    id<MTLBuffer> mVertexBuffer;
    id<MTLBuffer> mIndexBuffer;
    id<MTLBuffer> mUniformBuffer;
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
    mVertexBuffer = [mDevice newBufferWithBytes:verts length:sizeof(verts) options:MTLResourceStorageModeShared];
    mIndexBuffer = [mDevice newBufferWithBytes:indices length:sizeof(indices) options:MTLResourceStorageModeShared];
    mUniformBuffer = [mDevice newBufferWithLength:sizeof(Uniform) options:MTLResourceStorageModeShared];
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
    
    mPipeline = [mDevice newRenderPipelineStateWithDescriptor:descriptor error:&error];
    if (!mPipeline || error) {
        NSLog(@"Could not create pipeline: %@", error.classDescription);
        return;
    }
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
    matrix_float4x4 modelMatrix = matrix_multiply(TranslationMatrix(0, 0, -2), RotationMatrix(vector3(0.0f, 0.0f, 1.0f), degree * (3.14159 / 180.0)));
    matrix_float4x4 projMatrix = PerspectiveMatrixRH(60.0f*(3.14159/180.0f), 800.0f/600.0f, 0.1f, 100.0f);
    
    Uniform uniform;
    uniform.modelViewProj = matrix_multiply(projMatrix, modelMatrix);
    memcpy(mUniformBuffer.contents, &uniform, sizeof(Uniform));
    
    [encoder setRenderPipelineState:mPipeline];
    [encoder setVertexBuffer:mVertexBuffer offset:0 atIndex:0];
    [encoder setVertexBuffer:mUniformBuffer offset:0 atIndex:1];
    [encoder drawIndexedPrimitives:MTLPrimitiveTypeTriangle
                        indexCount:6
                         indexType:MTLIndexTypeUInt16
                       indexBuffer:mIndexBuffer
                 indexBufferOffset:0];
    
    [encoder endEncoding];
    [cmdBuff presentDrawable:view.currentDrawable];
    [cmdBuff commit];
}


- (void)mtkView: (nonnull MTKView *)view drawableSizeWillChange: (CGSize)size
{
    
}

@end
