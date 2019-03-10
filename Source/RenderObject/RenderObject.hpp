//  Copyright © 2019 Whelan Callahan. All rights reserved.
#pragma once
#include "../Math/Transform.hpp"

class RenderObject
{
public:
    
    RenderObject(RenderObject *parent);
    virtual ~RenderObject() {}
    
    void setTransform(const Transform &t);
    Transform getTransform() const;
    
    simd_float4x4 getGlobalTransformMatrix() const;
    
protected:
    
    RenderObject *mParent;
    Transform mTransform;
    
};
