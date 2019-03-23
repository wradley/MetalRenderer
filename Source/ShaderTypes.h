//  Copyright © 2019 Whelan Callahan. All rights reserved.
#pragma once
#include <simd/simd.h>

typedef struct {
    vector_float3 position;
    vector_float3 normal;
    vector_float2 uv;
    vector_int4   boneIDs;
    vector_float4 boneWeights;
} Vertex;


typedef struct {
    matrix_float4x4 projViewWorldMatrix;
    matrix_float3x3 normalMatrix;
    int             boneCount;
} Uniform;


typedef struct
{
    matrix_float4x4 globalTransform;
    matrix_float4x4 inverseGlobalTransform;
} BoneData;

typedef uint32_t Index;
