//  Copyright © 2019 Whelan Callahan. All rights reserved.
#include "RenderGroup.hpp"
#include "Model.hpp"
#include "Camera.hpp"


RenderGroup::RenderGroup(RenderGroup *parent) : RenderObject(parent)
{}


RenderGroup::~RenderGroup()
{
    for (Model *model : mModels) delete model;
    for (Camera *camera : mCameras) delete camera;
    for (RenderGroup *group : mGroups) delete group;
}


RenderGroup* RenderGroup::createGroup()
{
    RenderGroup *group = new RenderGroup(this);
    mGroups.push_back(group);
    return group;
}


Model* RenderGroup::createModel()
{
    Model *model = new Model(this);
    mModels.push_back(model);
    return model;
}


Camera* RenderGroup::createCamera()
{
    Camera *camera = new Camera(this);
    mCameras.push_back(camera);
    return camera;
}



