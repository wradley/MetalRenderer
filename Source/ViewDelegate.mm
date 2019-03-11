//  Copyright © 2019 Whelan Callahan. All rights reserved.

#import "ViewDelegate.h"
#import "Mesh/MeshLoader.hpp"
#import "Math/Math.hpp"
#import "Renderer.hpp"

//temp
//std::shared_ptr<CPUMesh> CreatePlane()
//{
//    Vertex verts[] = {
//        {simd_make_float3(-1,-1, 0), simd_make_float3(0, 0, 1), simd_make_float2(0, 0), simd_make_int3(1, 0, 0), simd_make_float3(1, 0, 0)},
//        {simd_make_float3( 1,-1, 0), simd_make_float3(0, 0, 1), simd_make_float2(0, 0), simd_make_int3(1, 0, 0), simd_make_float3(1, 0, 0)},
//        {simd_make_float3(-1, 1, 0), simd_make_float3(0, 0, 1), simd_make_float2(0, 0), simd_make_int3(2, 0, 0), simd_make_float3(1, 0, 0)},
//        {simd_make_float3( 1, 1, 0), simd_make_float3(0, 0, 1), simd_make_float2(0, 0), simd_make_int3(2, 0, 0), simd_make_float3(1, 0, 0)},
//    };
//
//    Index indices[] = {
//        0, 2, 3,
//        3, 1, 0
//    };
//
//    std::shared_ptr<CPUMesh> mesh(new CPUMesh);
//    mesh->vertices.resize(sizeof(verts)/sizeof(Vertex));
//    mesh->indices.resize(sizeof(indices)/sizeof(Index));
//    memcpy(&mesh->vertices[0], &verts, sizeof(verts));
//    memcpy(&mesh->indices[0], &indices, sizeof(indices));
//    return mesh;
//}

@implementation ViewDelegate {
    id<MTLDevice> mDevice;
    Renderer *mRenderer;
    Camera *mCamera;
    Model *mModel;
    float rot;
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
        
        mRenderer = new Renderer(mDevice, view);
        auto group = mRenderer->getRootGroup();
        
        mCamera = group->createCamera();
        mCamera->set(1.667f, DEG_TO_RAD(60.0f), 0.1f, 1000.0f);
        Transform t;
        t.position[2] = 10;
        t.rotation = Quat::FromAxisAngle(simd_make_float3(0, 1, 0), DEG_TO_RAD(180));
        mCamera->setTransform(t);
        mRenderer->setCamera(mCamera);
        
        mModel = group->createModel();
        std::vector<std::shared_ptr<CPUMesh>> meshDatas = MeshLoader::LoadFromFile("../../../sword.fbx");
        mModel->setMesh(mRenderer->createMesh(meshDatas[0]));
        //std::shared_ptr<CPUMesh> cube = CreatePlane();
        //mModel->setMesh(mRenderer->createMesh(cube));
        mModel->setColor(mRenderer->createTexture("../../../sword_c.png"));
    }
    
    return self;
}


- (void) dealloc
{
    delete mRenderer;
}


- (void)drawInMTKView: (nonnull MTKView *)view
{
//    Transform t = mCamera->getTransform();
//    rot += 0.01f;
//    t.rotation = Quat::FromAxisAngle(simd_make_float3(0, -1, 0), DEG_TO_RAD(30*sin(rot)));
//    mCamera->setTransform(t);
    mRenderer->draw(view);
}


- (void)mtkView: (nonnull MTKView *)view drawableSizeWillChange: (CGSize)size
{
    mCamera->setAspectRatio(size.width/size.height);
}

@end
