//  Copyright © 2019 Whelan Callahan. All rights reserved.
#include "QuaternionOperations.hpp"

namespace Quat
{
    simd_quatf FromAxisAngle(simd_float3 axis, float radians)
    {
        axis = simd_normalize(axis);
        float x = axis[0] * sin(radians/2.0f);
        float y = axis[1] * sin(radians/2.0f);
        float z = axis[2] * sin(radians/2.0f);
        float w = cos(radians/2.0f);
        return simd_quaternion(x, y, z, w);
    }
}
