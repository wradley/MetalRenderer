//  Copyright © 2019 Whelan Callahan. All rights reserved.
#pragma once
#include <simd/simd.h>

class Transform
{
public:
    
    Transform();
    ~Transform();
    
    simd_float4x4 getMatrix() const;
    
public:
    
    simd_float3 position;
    simd_quatf rotation;
    simd_float3 scale;
    
};
