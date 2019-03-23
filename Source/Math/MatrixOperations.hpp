//  Copyright © 2019 Whelan Callahan. All rights reserved.

#include <simd/simd.h>

namespace Matrix
{
    simd_float4x4 Scale(float s);
    simd_float4x4 Scale(float x, float y, float z);
    simd_float4x4 Translation(float tx, float ty, float tz);
    simd_float4x4 Translation(simd_float3 pos);
    simd_float4x4 Rotation(vector_float3 axis, float angle);
    simd_float4x4 Perspective(float fovyRadians, float aspect, float nearZ, float farZ);
    simd_float3x3 Normal(matrix_float4x4 viewModel);
    simd_float4x4 LookAtRH(simd_float3 target, simd_float3 eye, simd_float3 up);
}
