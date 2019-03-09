//  Copyright © 2019 Whelan Callahan. All rights reserved.

#import "ViewController.h"
#import "ViewDelegate.h"

@implementation ViewController {
    ViewDelegate *mViewDelegate;
}


- (void) viewDidLoad
{
    [super viewDidLoad];

    // Do any additional setup after loading the view.
    MTKView *view = (MTKView*) self.view;
    view.depthStencilPixelFormat = MTLPixelFormatDepth32Float;
    mViewDelegate = [[ViewDelegate alloc] initWithMtkView:view];
    view.delegate = mViewDelegate;
    
    [mViewDelegate mtkView:view drawableSizeWillChange:view.drawableSize];
}


- (void) setRepresentedObject: (id)representedObject
{
    [super setRepresentedObject:representedObject];

    // Update the view, if already loaded.
}


@end
