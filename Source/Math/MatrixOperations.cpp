//  Copyright © 2019 Whelan Callahan. All rights reserved.

#include "MatrixOperations.hpp"

simd_float4x4 IdentityMatrix()
{
    return (matrix_float4x4){{
        {1, 0, 0, 0},
        {0, 1, 0, 0},
        {0, 0, 1, 0},
        {0, 0, 0, 1}
    }};
}


simd_float4x4 ScaleMatrix(float s)
{
    return (matrix_float4x4){{
        {s, 0, 0, 0},
        {0, s, 0, 0},
        {0, 0, s, 0},
        {0, 0, 0, 1}
    }};
}


simd_float4x4 ScaleMatrix(float x, float y, float z)
{
    return (matrix_float4x4){{
        {x, 0, 0, 0},
        {0, y, 0, 0},
        {0, 0, z, 0},
        {0, 0, 0, 1}
    }};
}


simd_float4x4 TranslationMatrix(float tx, float ty, float tz)
{
    return (matrix_float4x4) {{
        { 1,   0,  0,  0 },
        { 0,   1,  0,  0 },
        { 0,   0,  1,  0 },
        { tx, ty, tz,  1 }
    }};
}


simd_float4x4 RotationMatrix(vector_float3 axis, float t)
{
    axis = vector_normalize(axis);
    float x = axis[0];
    float y = axis[1];
    float z = axis[2];
    float x2 = x * x;
    float y2 = y * y;
    float z2 = z * z;
    float ct = cosf(t);
    float st = sinf(t);
    float cti = 1.0f - ct;
    
    return (matrix_float4x4) {{
        {       ct + x2 * cti, y * x * cti + z * st, z * x * cti - y * st, 0},
        {x * y * cti - z * st,        ct + y2 * cti, z * y * cti + x * st, 0},
        {x * z * cti + y * st, y * z * cti - x * st,        ct + z2 * cti, 0},
        {                   0,                    0,                    0, 1}
    }};
}


simd_float4x4 PerspectiveMatrixRH(float fovyRadians, float aspect, float nearZ, float farZ)
{
    float ys = 1 / tanf(fovyRadians * 0.5);
    float xs = ys / aspect;
    float zs = farZ / (nearZ - farZ);
    
    return (matrix_float4x4) {{
        { xs,   0,          0,  0 },
        {  0,  ys,          0,  0 },
        {  0,   0,         zs, -1 },
        {  0,   0, nearZ * zs,  0 }
    }};
}


simd_float3x3 NormalMatrix(matrix_float4x4 viewModel)
{
    matrix_float3x3 sub;
    sub.columns[0] = simd_make_float3(viewModel.columns[0]);
    sub.columns[1] = simd_make_float3(viewModel.columns[1]);
    sub.columns[2] = simd_make_float3(viewModel.columns[2]);
    return matrix_transpose(matrix_invert(sub));
}