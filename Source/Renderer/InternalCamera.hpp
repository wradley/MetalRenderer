//  Copyright © 2019 Whelan Callahan. All rights reserved.
#pragma once
#include "../Math/Transform.hpp"
#include "../Math/MatrixOperations.hpp"

class InternalCamera
{
public:

    InternalCamera(uint64_t Parent = 0);
    InternalCamera(const InternalCamera &c);
    InternalCamera& operator= (const InternalCamera &c);
    ~InternalCamera();

    inline matrix_float4x4 calcPerspectiveMatrix()
    {
        return Matrix::Perspective(fieldOfView, aspectRatio, nearPlane, farPlane);
    }

    uint64_t parent;
    float fieldOfView;
    float aspectRatio;
    float nearPlane;
    float farPlane;

};
