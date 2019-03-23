//  Copyright © 2019 Whelan Callahan. All rights reserved.
#pragma once
#include <vector>
#include "../Math/Transform.hpp"

class InternalGroup
{
public:

    InternalGroup(uint64_t parent = 0);
    InternalGroup(const InternalGroup &g);
    InternalGroup& operator= (const InternalGroup &g);
    InternalGroup(InternalGroup &&g);
    ~InternalGroup();

    uint64_t parent;
    uint64_t cameraAttachment;
    uint64_t modelAttachment;
    Transform transform;
    std::vector<uint64_t> childGroups;
};
