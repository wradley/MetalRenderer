//  Copyright © 2019 Whelan Callahan. All rights reserved.
#include "Transform.hpp"
#include "MatrixOperations.hpp"


Transform::Transform()
{}


Transform::~Transform()
{}


simd_float4x4 Transform::getMatrix() const
{
    simd_float4x4 s = ScaleMatrix(scale[0], scale[1], scale[2]);
    simd_float4x4 r = simd_matrix4x4(rotation);
    simd_float4x4 t = TranslationMatrix(position[0], position[1], position[2]);
    
    return simd_mul(simd_mul(t, r), s);
}
