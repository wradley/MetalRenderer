#pragma once

#include <simd/simd.h>

typedef struct {
    vector_float4 color;
    vector_float3 position;
} Vertex;


typedef struct {
    matrix_float4x4 modelViewProj;
} Uniform;
