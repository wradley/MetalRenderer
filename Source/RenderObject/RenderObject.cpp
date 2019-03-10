//  Copyright © 2019 Whelan Callahan. All rights reserved.
#include "RenderObject.hpp"


RenderObject::RenderObject(RenderObject *parent) : mParent(parent)
{}


void RenderObject::setTransform(const Transform &t)
{
    mTransform = t;
}


Transform RenderObject::getTransform() const
{
    return mTransform;
}


simd_float4x4 RenderObject::getGlobalTransformMatrix() const
{
    if (mParent) {
        return matrix_multiply(mParent->getGlobalTransformMatrix(), mTransform.getMatrix());
    }
    
    return mTransform.getMatrix();
}
