//  Copyright © 2019 Whelan Callahan. All rights reserved.
#pragma once
#include "InternalGroup.hpp"
#include "InternalModel.hpp"
#include "InternalCamera.hpp"

class Renderer;
#define NIL_INDEX 0

class RendererObjectDelegate
{
public:

    RendererObjectDelegate(Renderer *renderer);
    ~RendererObjectDelegate() {}

    uint64_t createGroup(uint64_t parent);
    void setGroupParent(uint64_t group, uint64_t parent);
    void setGroupTransform(uint64_t group, const Transform &t);
    void setGroupCameraAttachment(uint64_t group, uint64_t camera);
    void setGroupModelAttachment(uint64_t group, uint64_t model);
    //void releaseGroup(uint64_t group);

    uint64_t createCamera(uint64_t parent);
    void setCameraFieldOfView(uint64_t camera, float fov);
    void setCameraAspectRatio(uint64_t camera, float ratio);
    void setCameraNearPlane(uint64_t camera, float distance);
    void setCameraFarPlane(uint64_t camera, float distance);
    //void releaseCamera(uint64_t camera);

    uint64_t createModel(uint64_t parent);
    void setModelMesh(uint64_t model, const Mesh &mesh);
    void setModelMesh(uint64_t model, const Mesh &mesh, const Skeleton &skeleton);
    void setModelAnimations(uint64_t model, const Animation *const animations, uint32_t count);
    void setModelAnimation(uint64_t model, const std::string &name);
    void setModelColorTexture(uint64_t model, uint64_t texture);
    //void releaseModel(uint64_t model);

private:

    Renderer *mRenderer;
    
};
