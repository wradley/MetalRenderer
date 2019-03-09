//  Copyright © 2019 Whelan Callahan. All rights reserved.
#include <objc/objc-runtime.h>
#include "../Math/Transform.hpp"

class GPUMesh
{
public:
    
    unsigned int uniformOffset;
    unsigned int indexCount;
    id vertices;
    id indices;
    Transform transform;
    
};
