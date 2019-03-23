//  Copyright © 2019 Whelan Callahan. All rights reserved.
#include "RendererObjectDelegate.hpp"
#include "Renderer.hpp"
#import <Metal/Metal.h>

RendererObjectDelegate::RendererObjectDelegate(Renderer *renderer) :
    mRenderer(renderer)
{}


uint64_t RendererObjectDelegate::createGroup(uint64_t parent)
{
    uint64_t newIndex = mRenderer->mGroups.size();
    InternalGroup group(parent);
    if (parent != NIL_INDEX) {
        mRenderer->mGroups[parent].childGroups.push_back(newIndex);
    }

    mRenderer->mGroups.push_back(group);
    return newIndex;
}


void RendererObjectDelegate::setGroupParent(uint64_t group, uint64_t parent)
{
    mRenderer->mGroups[group].parent = parent;
}


void RendererObjectDelegate::setGroupTransform(uint64_t group, const Transform &t)
{
    mRenderer->mGroups[group].transform = t;
}


void RendererObjectDelegate::setGroupCameraAttachment(uint64_t group, uint64_t camera)
{
    mRenderer->mGroups[group].cameraAttachment = camera;
    mRenderer->mCameras[camera].parent = group;
}


void RendererObjectDelegate::setGroupModelAttachment(uint64_t group, uint64_t model)
{
    mRenderer->mGroups[group].modelAttachment = model;
    mRenderer->mCameras[model].parent = group;
}


uint64_t RendererObjectDelegate::createCamera(uint64_t parent)
{
    uint64_t newIndex = mRenderer->mCameras.size();
    InternalCamera camera(parent);
    if (parent != NIL_INDEX) {
        mRenderer->mGroups[parent].childGroups.push_back(newIndex);
    }

    mRenderer->mCameras.push_back(camera);
    return newIndex;
}


void RendererObjectDelegate::setCameraFieldOfView(uint64_t camera, float fov)
{
    mRenderer->mCameras[camera].fieldOfView = fov;
}


void RendererObjectDelegate::setCameraAspectRatio(uint64_t camera, float ratio)
{
    mRenderer->mCameras[camera].aspectRatio = ratio;
}


void RendererObjectDelegate::setCameraNearPlane(uint64_t camera, float distance)
{
    mRenderer->mCameras[camera].nearPlane = distance;
}


void RendererObjectDelegate::setCameraFarPlane(uint64_t camera, float distance)
{
    mRenderer->mCameras[camera].farPlane = distance;
}


uint64_t RendererObjectDelegate::createModel(uint64_t parent)
{
    uint64_t newIndex = mRenderer->mModels.size();
    InternalModel model(parent);
    if (parent != NIL_INDEX) {
        mRenderer->mGroups[parent].childGroups.push_back(newIndex);
    }

    mRenderer->mModels.push_back(model);
    return newIndex;
}


void RendererObjectDelegate::setModelMesh(uint64_t model, const Mesh &mesh)
{
    mRenderer->mModels[model].mesh = mesh;
}


void RendererObjectDelegate::setModelMesh(uint64_t model, const Mesh &mesh, const Skeleton &skeleton)
{
    mRenderer->mModels[model].skeleton = skeleton;
    mRenderer->mModels[model].mesh = mesh;
    
    // calculate bone inverse matrices
    id<MTLBuffer> buffer = mRenderer->mBoneBuffers[mesh.boneBuffer];
    void *mem = (char*)buffer.contents + mesh.boneBufferOffset;
    mRenderer->mModels[model].calculateBoneInverseMatrices(mem);
}


void RendererObjectDelegate::setModelAnimations(uint64_t model, const Animation *const animations, uint32_t count)
{
    mRenderer->mModels[model].animationCount = count;
    mRenderer->mModels[model].animations = new Animation[count];
    //memcpy(mModels[model].animations, animations, sizeof(Animation) * count);
    for (int i = 0; i < count; ++i) {
        mRenderer->mModels[model].animations[i] = animations[i];
    }
}


void RendererObjectDelegate::setModelAnimation(uint64_t model, const std::string &name)
{
    for (int i = 0; i < mRenderer->mModels[model].animationCount; ++i) {
        mRenderer->mModels[model].currAnimation = i;
        if (mRenderer->mModels[model].animations[i].name == name) break;
    }
}


void RendererObjectDelegate::setModelColorTexture(uint64_t model, uint64_t texture)
{
    mRenderer->mModels[model].colorTexture = texture;
}
