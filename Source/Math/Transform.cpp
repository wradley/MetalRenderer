//  Copyright © 2019 Whelan Callahan. All rights reserved.
#include "Transform.hpp"
#include "MatrixOperations.hpp"
#include "QuaternionOperations.hpp"


Transform::Transform()
{
    position = simd_make_float3(0.0f, 0.0f, 0.0f);
    rotation = Quat::FromAxisAngle(simd_make_float3(0.0f, 0.0f, -1.0f), 0.0f);
    scale = simd_make_float3(1.0f, 1.0f, 1.0f);
}


Transform::Transform(const Transform &t) :
    position(t.position), rotation(t.rotation), scale(t.scale)
{}


Transform Transform::operator= (const Transform &t)
{
    position = t.position;
    rotation = t.rotation;
    scale = t.scale;
    return *this;
}


Transform::~Transform()
{}


simd_float4x4 Transform::getMatrix() const
{
    simd_float4x4 s = Matrix::Scale(scale[0], scale[1], scale[2]);
    simd_float4x4 r = simd_matrix4x4(rotation);
    simd_float4x4 t = Matrix::Translation(position[0], position[1], position[2]);
    
    return simd_mul(simd_mul(t, r), s);
}
