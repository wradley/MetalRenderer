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


MeshData CreatePlane()
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

    MeshData mesh;
    mesh.vertices.resize(sizeof(verts)/sizeof(Vertex));
    mesh.indices.resize(sizeof(indices)/sizeof(Index));
    memcpy(&mesh.vertices[0], &verts, sizeof(verts));
    memcpy(&mesh.indices[0], &indices, sizeof(indices));
    
    Skeleton skeleton = CreateSkeleton();
    mesh.skeleton = skeleton;
    return mesh;
}


Animation CreateAnimation()
{
    Channel channels[2];
    
    // parent bone
    KeyFrame parentFrame;
    parentFrame.frameNumber = 0;
    parentFrame.transform = AnimationTransform();
    channels[0] = Channel(&parentFrame, 1);
    
    // child bone
    KeyFrame childFrames[3];
    childFrames[0].frameNumber = 0;
    childFrames[0].transform = AnimationTransform();
    //childFrames[0].transform.position.x = -1;
    childFrames[0].transform.rotation = Quat::FromAxisAngle(simd_make_float3(0, 0, 1), DEG_TO_RAD(45));
    childFrames[1].frameNumber = 11;
    childFrames[1].transform = AnimationTransform();
    //childFrames[1].transform.position.x = 1;
    childFrames[2] = childFrames[0];
    childFrames[2].frameNumber = 24;
    channels[1] = Channel(childFrames, 3);
    
//    const Channel *channels,
//    uint32_t channelCount,
//    float fps,
//    uint32_t totalFrames,
//    const std::string &name
    
    return Animation(channels, 2, 24, 24, "Test Animation");
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
        t.position[2] = 8;
        t.rotation = Quat::FromAxisAngle(simd_make_float3(0, 1, 0), DEG_TO_RAD(180));
        mRenderer->objectDelegate->setGroupTransform(mCamera, t);
        mRenderer->setCamera(mCameraAttachment);

        mModel = mRenderer->objectDelegate->createGroup(rootGroup);
        mModelAttatchment = mRenderer->objectDelegate->createModel(mModel);
        auto meshDatas = std::make_shared<std::vector<MeshData>>();
        meshDatas->push_back(CreatePlane());
        
        std::vector<Mesh> meshes = mRenderer->resourceDelegate->createMeshes(meshDatas);
        mRenderer->objectDelegate->setModelMesh(mModelAttatchment, meshes[0], CreateSkeleton());
        
//        auto meshDatas = MeshLoader::LoadFromFile("../../../Robot.fbx");
//        std::vector<Mesh> meshes = mRenderer->resourceDelegate->createMeshes(meshDatas);
//        mRenderer->objectDelegate->setModelMesh(mModelAttatchment, meshes[0]);
        mRenderer->objectDelegate->setModelColorTexture(mModelAttatchment, mRenderer->resourceDelegate->createTexture("../../../sword_c.png"));
//        Transform modelTransform;
//        modelTransform.rotation = Quat::FromAxisAngle(simd_make_float3(1, 0, 0), DEG_TO_RAD(-90));
//        mRenderer->objectDelegate->setGroupTransform(mModel, modelTransform);
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
    static auto start = std::chrono::high_resolution_clock::now();
    //auto duration = std::chrono::high_resolution_clock::now().time_since_epoch();
    auto now = std::chrono::high_resolution_clock::now();
    auto micro = std::chrono::duration_cast<std::chrono::microseconds>(now - start).count();
    double time = ((double)micro) / 1000000.0;
    
//    static float deg = 0.0f;
//    deg = 10.0 * time;
//    Transform t;
//    t.rotation = Quat::FromAxisAngle(simd_make_float3(1, 0, 0), DEG_TO_RAD(-90));
//    t.rotation = simd_mul(Quat::FromAxisAngle(simd_make_float3(0, 1, 0), DEG_TO_RAD(deg)), t.rotation);
//    mRenderer->objectDelegate->setGroupTransform(mModel, t);
    
    mRenderer->draw(view, time);
}


- (void)mtkView: (nonnull MTKView *)view drawableSizeWillChange: (CGSize)size
{
    mRenderer->objectDelegate->setCameraAspectRatio(mCameraAttachment, size.width/size.height);
}

@end
