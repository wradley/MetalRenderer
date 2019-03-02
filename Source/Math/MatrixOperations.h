//  Copyright © 2019 Whelan Callahan. All rights reserved.

#import <simd/simd.h>

matrix_float4x4 IdentityMatrix();
matrix_float4x4 TranslationMatrix(float tx, float ty, float tz);
matrix_float4x4 RotationMatrix(vector_float3 axis, float angle);
matrix_float4x4 PerspectiveMatrixRH(float fovyRadians, float aspect, float nearZ, float farZ);
