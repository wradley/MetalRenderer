//  Copyright © 2019 Whelan Callahan. All rights reserved.
#include "Camera.hpp"
#include "RenderGroup.hpp"
#include "../Math/MatrixOperations.hpp"
#include "../Math/QuaternionOperations.hpp"
#include "../Math/MathDefines.h"

Camera::Camera(RenderGroup *g) : RenderObject(g)
{}


Camera::~Camera()
{}


void Camera::set(float aspectRatio, float fieldOfView, float nearPlane, float farPlane)
{
    mAspectRatio = aspectRatio;
    mFieldOfView = fieldOfView;
    mNearPlane = nearPlane;
    mFarPlane = farPlane;
}


void Camera::setAspectRatio(float a)
{
    mAspectRatio = a;
}


void Camera::setFieldOfView(float f)
{
    mFieldOfView = f;
}


void Camera::setNearPlane(float n)
{
    mNearPlane = n;
}


void Camera::setFarPlane(float f)
{mFarPlane = f;}


float Camera::getAspectRatio() const
{
    return mAspectRatio;
}


float Camera::getFieldOfView() const
{
    return mFieldOfView;
}


float Camera::getNearPlane() const
{
    return mNearPlane;
}


float Camera::getFarPlane() const
{
    return mFarPlane;
}


simd_float4x4 Camera::getGlobalViewMatrix() const
{
    simd_float4x4 mat = getGlobalTransformMatrix();
    simd_float4 myPos = simd_mul(mat, simd_make_float4(0.0f, 0.0f, 0.0f, 1.0f));
    simd_float4 target = simd_mul(mat, simd_make_float4(0.0f, 0.0f, 1.0f, 1.0f));
    
    return Matrix::LookAtRH(simd_make_float3(myPos), simd_make_float3(target), simd_make_float3(0.0f, 1.0f, 0.0f));
}
