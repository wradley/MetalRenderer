//  Copyright © 2019 Whelan Callahan. All rights reserved.

#import "ViewDelegate.h"

@implementation ViewDelegate {
    id<MTLDevice> mDevice;
    id<MTLCommandQueue> mCmdQ;
}

-(instancetype) initWithMtkView: (MTKView *)view
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
    }
    
    return self;
}


- (void)drawInMTKView: (nonnull MTKView *)view
{
    id<MTLCommandBuffer> cmdBuff = [mCmdQ commandBuffer];
    
    MTLRenderPassDescriptor *renderPassDescriptor = view.currentRenderPassDescriptor;
    renderPassDescriptor.colorAttachments[0].clearColor = MTLClearColorMake(63.0/255, 133.0/255, 193.0/255, 1.0);
    id<MTLRenderCommandEncoder> encoder = [cmdBuff renderCommandEncoderWithDescriptor:renderPassDescriptor];
    
    [encoder endEncoding];
    [cmdBuff presentDrawable:view.currentDrawable];
    [cmdBuff commit];
}


- (void)mtkView: (nonnull MTKView *)view drawableSizeWillChange: (CGSize)size
{
    
}

@end
