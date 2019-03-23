//  Copyright © 2019 Whelan Callahan. All rights reserved.
#pragma once
#include <objc/objc-runtime.h>
#include "../Math/Transform.hpp"

class Mesh
{
public:

    // Vertices and indices are stored in the same buffer
    // [..vertices..|..indices..]
    uint32_t vertexIndexBuffer;
    uint32_t indexOffset;
    uint32_t indexCount;

    uint32_t uniformBuffer;
    uint32_t uniformBufferOffset;
    uint32_t boneBuffer;
    uint32_t boneBufferOffset;

};
