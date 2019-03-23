//  Copyright © 2019 Whelan Callahan. All rights reserved.

#import "ViewDelegate.h"
#import "Mesh/MeshLoader.hpp"
#import "Math/Math.hpp"
#import "Renderer/Renderer.hpp"
#include <chrono>

// temp
Skeleton CreateSkeleton()
{
    auto parents = new uint32_t[2];
    parents[0] = 0;
    parents[1] = 0;
    
    auto transforms = new AnimationTransform[2];
    transforms[0].position = simd_make_float3(0, 0, 0);
    transforms[1].position = simd_make_float3(0, 1, 0);
    
    return Skeleton(parents, transforms, 2);
}


std::shared_ptr<MeshData> CreatePlane()
{
    Vertex verts[] = {
        {simd_make_float3(-1,-1, 0), simd_make_float3(0, 0, 1), simd_make_float2(0, 0), simd_make_int4(0, 0, 0, 0), simd_make_float4(1, 0, 0, 0)},
        {simd_make_float3( 1,-1, 0), simd_make_float3(0, 0, 1), simd_make_float2(1, 0), simd_make_int4(0, 0, 0, 0), simd_make_float4(1, 0, 0, 0)},
        {simd_make_float3(-1, 1, 0), simd_make_float3(0, 0, 1), simd_make_float2(0, 1), simd_make_int4(1, 0, 0, 0), simd_make_float4(1, 0, 0, 0)},
        {simd_make_float3( 1, 1, 0), simd_make_float3(0, 0, 1), simd_make_float2(1, 1), simd_make_int4(1, 0, 0, 0), simd_make_float4(1, 0, 0, 0)},
    };

    Index indices[] = {
        0, 2, 3,
        3, 1, 0
    };

    std::shared_ptr<MeshData> mesh(new MeshData);
    mesh->vertices.resize(sizeof(verts)/sizeof(Vertex));
    mesh->indices.resize(sizeof(indices)/sizeof(Index));
    memcpy(&mesh->vertices[0], &verts, sizeof(verts));
    memcpy(&mesh->indices[0], &indices, sizeof(indices));
    
    Skeleton skeleton = CreateSkeleton();
    mesh->skeleton = skeleton;
    return mesh;
}


Animation CreateAnimation()
{
    KeyFrame frames[3];
    
    KeyFrame &frame0 = frames[0];
    frame0.frameNumber = 0;
    frame0.transformCount = 2;
    frame0.transforms = new AnimationTransform[2];
    frame0.transforms[0].position = simd_make_float3( 0, 0, 0);
    frame0.transforms[1].position = simd_make_float3(-1, 0, 0);
    frame0.transforms[0].orientation = Quat::FromAxisAngle(simd_make_float3(0, 0, 1), DEG_TO_RAD(45));
    frame0.transforms[1].orientation = Quat::FromAxisAngle(simd_make_float3(0, 0, 1), DEG_TO_RAD(45));
    
    KeyFrame &frame1 = frames[1];
    frame1.frameNumber = 12;
    frame1.transformCount = 2;
    frame1.transforms = new AnimationTransform[2];
    frame1.transforms[1].orientation = Quat::FromAxisAngle(simd_make_float3(0, 0, 1), DEG_TO_RAD(-45));
    frame1.transforms[0].position = simd_make_float3(0, 0, 0);
    frame1.transforms[1].position = simd_make_float3(1, 0, 0);
    
    frames[2] = frames[0];
    frames[2].frameNumber = 23;
    
    //KeyFrame *keyFrames, uint32_t keyFrameCount, uint64_t fps, uint64_t totalFrames, const std::string &name);
    return Animation(frames, 3, 12, 24, "Test Animation");
}


@implementation ViewDelegate {
    id<MTLDevice> mDevice;
    Renderer *mRenderer;
    uint64_t mCamera;
    uint64_t mCameraAttachment;
    uint64_t mModel;
    uint64_t mModelAttatchment;
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
        uint64_t rootGroup = mRenderer->getRootGroup();
        
        mCamera = mRenderer->objectDelegate->createGroup(rootGroup);
        mCameraAttachment = mRenderer->objectDelegate->createCamera(mCamera);
        Transform t;
        t.position[2] = 5;
        t.rotation = Quat::FromAxisAngle(simd_make_float3(0, 1, 0), DEG_TO_RAD(180));
        mRenderer->objectDelegate->setGroupTransform(mCamera, t);
        mRenderer->setCamera(mCameraAttachment);
        
        mModel = mRenderer->objectDelegate->createGroup(rootGroup);
        mModelAttatchment = mRenderer->objectDelegate->createModel(mModel);
        std::shared_ptr<MeshData> cube = CreatePlane();
        std::vector<std::shared_ptr<MeshData>> meshDatas;
        meshDatas.push_back(cube);
        std::vector<Mesh> meshes = mRenderer->resourceDelegate->createMeshes(meshDatas);
        mRenderer->objectDelegate->setModelMesh(mModelAttatchment, meshes[0], CreateSkeleton());
        mRenderer->objectDelegate->setModelColorTexture(mModelAttatchment, mRenderer->resourceDelegate->createTexture("../../../sword_c.png"));
        Animation animation = CreateAnimation();
        mRenderer->objectDelegate->setModelAnimations(mModelAttatchment, &animation, 1);
        mRenderer->objectDelegate->setModelAnimation(mModelAttatchment, "Test Animation");
    }
    
    return self;
}


- (void) dealloc
{
    delete mRenderer;
}


- (void)drawInMTKView: (nonnull MTKView *)view
{  
    auto duration = std::chrono::high_resolution_clock::now().time_since_epoch();
    auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
    mRenderer->draw(view, millis);
}


- (void)mtkView: (nonnull MTKView *)view drawableSizeWillChange: (CGSize)size
{
    mRenderer->objectDelegate->setCameraAspectRatio(mCameraAttachment, size.width/size.height);
}

@end
