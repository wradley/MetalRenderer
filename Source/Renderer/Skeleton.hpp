//  Copyright © 2019 Whelan Callahan. All rights reserved.
#pragma once
#include <cstdint>
#include "Animation.hpp"

class Skeleton
{
public:

    Skeleton();
    Skeleton(const uint32_t *parents, AnimationTransform *transforms, uint32_t boneCount);
    Skeleton(const Skeleton &s);
    Skeleton(Skeleton &&s);
    Skeleton& operator= (const Skeleton &s);
    ~Skeleton();

    // The content of each bone is the index to its parent bone. Bones
    // ordered such that a child is always after a parent with the parent of
    // all parents first in the array with a value of itself.
    uint32_t *parents;
    
    // expects transforms to be in bone space (offsets from parent)
    AnimationTransform *transforms;
    uint32_t boneCount;
};
