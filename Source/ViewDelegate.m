//  Copyright © 2019 Whelan Callahan. All rights reserved.

#import "ViewDelegate.h"
#import "ShaderTypes.h"

Vertex verts[] = {
    {{1.0f, 0.0f, 0.0f, 1.0f}, {-1.0f, -1.0f, 0.0f}},
    {{0.0f, 1.0f, 0.0f, 1.0f}, { 0.0f,  1.0f, 0.0f}},
    {{0.0f, 0.0f, 1.0f, 1.0f}, { 1.0f, -1.0f, 0.0f}}
};

@implementation ViewDelegate {
    id<MTLDevice> mDevice;
    id<MTLCommandQueue> mCmdQ;
    id<MTLRenderPipelineState> mPipeline;
    
    // temp
    id<MTLBuffer> mVertexBuffer;
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
        [self _buildVertexBuffer];
    }
    
    return self;
}


- (void)_buildVertexBuffer
{
    mVertexBuffer = [mDevice newBufferWithBytes:verts length:sizeof(verts) options:MTLResourceStorageModeShared];
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
    
    [encoder setRenderPipelineState:mPipeline];
    [encoder setVertexBuffer:mVertexBuffer offset:0 atIndex:0];
    [encoder drawPrimitives:MTLPrimitiveTypeTriangle vertexStart:0 vertexCount:3];
    
    [encoder endEncoding];
    [cmdBuff presentDrawable:view.currentDrawable];
    [cmdBuff commit];
}


- (void)mtkView: (nonnull MTKView *)view drawableSizeWillChange: (CGSize)size
{
    
}

@end
