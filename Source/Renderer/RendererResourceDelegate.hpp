//  Copyright © 2019 Whelan Callahan. All rights reserved.
#pragma once
#include <objc/runtime.h>
#include "../Mesh/MeshData.hpp"
#include "Mesh.hpp"


class RendererResourceDelegate
{
public:

    RendererResourceDelegate(id device,
                             id cmdQ,
                             std::vector<id> &vertexIndexBuffers,
                             std::vector<id> &uniformBuffers,
                             std::vector<id> &boneBuffers,
                             std::vector<id> &textures);
    ~RendererResourceDelegate();

    std::vector<Mesh> createMeshes(std::shared_ptr<std::vector<MeshData>> datas);
    uint64_t createTexture(const char *fp);

private:
    
    void assignUniform(uint32_t &buffer, uint32_t &offset);
    void assignBoneMatrices(uint32_t boneCount, uint32_t &buffer, uint32_t &offset);
    
    id mDevice;
    id mCmdQ;
    
    std::vector<id> &mVertexIndexBuffers;
    
    std::vector<id> &mUniformBuffers;
    unsigned int mNextFreeUniform;
    const unsigned int UniformBufferSize = sizeof(Uniform) * 100;
    
    std::vector<id> &mBoneBuffers;
    unsigned int mNextFreeBoneOffset;
    const unsigned int BoneBufferSize = sizeof(simd_float4x4) * 100 * 100;
    
    std::vector<id> &mTextures;
};
