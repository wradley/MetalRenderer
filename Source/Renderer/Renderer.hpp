//  Copyright © 2019 Whelan Callahan. All rights reserved.
#pragma once
#include <objc/runtime.h>
#include <vector>
#include <simd/simd.h>
#include <memory>
#include <cstdint>
#include "InternalCamera.hpp"
#include "InternalGroup.hpp"
#include "InternalModel.hpp"
#include "RendererObjectDelegate.hpp"
#include "RendererResourceDelegate.hpp"


class Renderer
{
public:

    Renderer(id device, id view);
    ~Renderer();

    uint64_t getRootGroup();
    void setCamera(uint64_t cam);
    void draw(id view, uint64_t time);

    std::unique_ptr<RendererObjectDelegate> objectDelegate;
    std::unique_ptr<RendererResourceDelegate> resourceDelegate;

private:

    void initializeInternals();
    void drawModels(id encoder, uint64_t time);
    void updateAnimations(uint64_t time);
    void updateUniforms();

    simd_float4x4 calcWorldMatrix(const InternalGroup &g) const;

    friend class RendererObjectDelegate;
    std::vector<InternalGroup> mGroups;
    std::vector<InternalModel> mModels;
    std::vector<InternalCamera> mCameras;
    uint64_t mCurrCamera;
    
    std::vector<id> mVertexIndexBuffers;
    std::vector<id> mUniformBuffers;
    std::vector<id> mBoneBuffers;
    std::vector<id> mTextures;

    struct Data;
    Data *mData;

private:

    Renderer(const Renderer &) = delete;
    Renderer& operator= (const Renderer &) = delete;
    Renderer(Renderer &&) = delete;

};
