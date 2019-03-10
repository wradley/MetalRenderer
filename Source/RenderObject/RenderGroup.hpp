//  Copyright © 2019 Whelan Callahan. All rights reserved.
#pragma once
#include <vector>
#include "RenderObject.hpp"

class Model;
class Camera;
class Renderer;

class RenderGroup : public RenderObject
{
public:
    
    RenderGroup* createGroup();
    Model* createModel();
    Camera* createCamera();
    
private:
    
    friend class Renderer;
    RenderGroup(RenderGroup *parent);
    ~RenderGroup();
    
private:
    
    std::vector<RenderGroup*> mGroups;
    std::vector<Model*> mModels;
    std::vector<Camera*> mCameras;
    
private:
    
    RenderGroup(const RenderGroup &) = delete;
    RenderGroup operator= (const RenderGroup &) = delete;
    RenderGroup(RenderGroup &&) = delete;
    
};
