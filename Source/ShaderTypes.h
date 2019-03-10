//  Copyright © 2019 Whelan Callahan. All rights reserved.
#pragma once

#include <simd/simd.h>

typedef struct {
    vector_float3 position;
    vector_float3 normal;
    vector_float2 uv;
} Vertex;


typedef struct {
    matrix_float4x4 projViewModelMatrix;
    matrix_float3x3 normalMatrix;
} Uniform;

typedef uint32_t Index;
