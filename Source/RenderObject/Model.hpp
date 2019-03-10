//  Copyright © 2019 Whelan Callahan. All rights reserved.
#pragma once
#include <memory>
#include "RenderObject.hpp"

class Renderer;
class RenderGroup;
class GPUMesh;
class Texture;

class Model : public RenderObject
{
public:
    
    void setMesh(std::shared_ptr<GPUMesh> mesh);
    void setColor(std::shared_ptr<Texture> texture);
    
private:
    
    std::shared_ptr<GPUMesh> mMesh;
    std::shared_ptr<Texture> mColor;
    
private:
    
    friend class RenderGroup;
    friend class Renderer;
    Model(RenderGroup *g);
    ~Model();
    
private:
    
    Model(const Model &) = delete;
    Model& operator= (const Model &) = delete;
    Model(Model &&) = delete;
    
};
