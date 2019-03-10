//  Copyright © 2019 Whelan Callahan. All rights reserved.
#pragma once
#include "RenderObject.hpp"

class RenderGroup;

class Camera : public RenderObject
{
public:
    
    void set(float aspectRatio, float fieldOfView, float nearPlane, float farPlane);
    void setAspectRatio(float a);
    void setFieldOfView(float f);
    void setNearPlane(float n);
    void setFarPlane(float f);
    
    float getAspectRatio() const;
    float getFieldOfView() const;
    float getNearPlane() const;
    float getFarPlane() const;
    
    simd_float4x4 getGlobalViewMatrix() const;
    
private:
    
    float mAspectRatio;
    float mFieldOfView;
    float mNearPlane;
    float mFarPlane;
    
private:
    
    friend class RenderGroup;
    Camera(RenderGroup *g);
    ~Camera();
    
private:
    
    Camera(const Camera &) = delete;
    Camera& operator= (const Camera &) = delete;
    Camera(Camera &&) = delete;
    
};
