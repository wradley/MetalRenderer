//  Copyright © 2019 Whelan Callahan. All rights reserved.

#include <simd/simd.h>

simd_float4x4 IdentityMatrix();
simd_float4x4 ScaleMatrix(float s);
simd_float4x4 ScaleMatrix(float x, float y, float z);
simd_float4x4 TranslationMatrix(float tx, float ty, float tz);
simd_float4x4 RotationMatrix(vector_float3 axis, float angle);
simd_float4x4 PerspectiveMatrixRH(float fovyRadians, float aspect, float nearZ, float farZ);
simd_float3x3 NormalMatrix(matrix_float4x4 viewModel);
