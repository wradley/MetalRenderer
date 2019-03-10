//  Copyright © 2019 Whelan Callahan. All rights reserved.
#pragma once
#include <objc/runtime.h>
#include <memory>
#include "RenderObject/RenderGroup.hpp"
#include "RenderObject/Model.hpp"
#include "RenderObject/Camera.hpp"
#include "ShaderTypes.h"
#include "Mesh/CPUMesh.hpp"

class Renderer
{
public:
    
    Renderer(id device, id view);
    ~Renderer();
    
    RenderGroup* getRootGroup();
    void setCamera(Camera *c);
    void draw(id view);
    
    std::shared_ptr<GPUMesh> createMesh(std::shared_ptr<CPUMesh> cpuMesh);
    std::shared_ptr<Texture> createTexture(const char *fp);
    
private:
    
    RenderGroup *mGroup;
    Camera *mCamera;
    
    struct Data;
    Data *mData;
    
private:
    
    // temp: recursively draw render groups
    void drawModels(RenderGroup *g, id Encoder);
    
    // temp: recursively update model uniforms
    void updateModelUniforms(simd_float4x4 projViewMat, Uniform *uniforms, RenderGroup *g);
    
private:
    
    Renderer(const Renderer &) = delete;
    Renderer& operator= (const Renderer &) = delete;
    Renderer(Renderer &&) = delete;
    
};
