//  Copyright © 2019 Whelan Callahan. All rights reserved.
#include "InternalCamera.hpp"
#include "../Math/Math.hpp"


InternalCamera::InternalCamera(uint64_t Parent) :
    parent(Parent),
    fieldOfView(DEG_TO_RAD(60.0f)),
    aspectRatio(1.667f),
    nearPlane(0.1f),
    farPlane(1000.0f)
{}


InternalCamera::InternalCamera(const InternalCamera &c) :
    parent(c.parent),
    fieldOfView(c.fieldOfView),
    aspectRatio(c.aspectRatio),
    nearPlane(c.nearPlane),
    farPlane(c.farPlane)
{}


InternalCamera& InternalCamera::operator= (const InternalCamera &c)
{
    parent = c.parent;
    fieldOfView = c.fieldOfView;
    aspectRatio = c.aspectRatio;
    nearPlane = c.nearPlane;
    farPlane = c.farPlane;
    return *this;
}


InternalCamera::~InternalCamera()
{}
