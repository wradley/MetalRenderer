//  Copyright © 2019 Whelan Callahan. All rights reserved.

#import <Foundation/Foundation.h>
#import <MetalKit/MetalKit.h>

@interface ViewDelegate : NSObject<MTKViewDelegate>

-(instancetype) initWithMtkView: (MTKView *)view;

@end
