//  Copyright © 2019 Whelan Callahan. All rights reserved.
#include "Skeleton.hpp"
#include <cstring>


Skeleton::Skeleton() :
    parents(nullptr),
    transforms(nullptr),
    boneCount(0)
{}


Skeleton::Skeleton(const uint32_t *Parents, AnimationTransform *Transforms, uint32_t BoneCount) :
    parents(new uint32_t[BoneCount]),
    transforms(new AnimationTransform[BoneCount]),
    boneCount(BoneCount)
{
    for (int i = 0; i < boneCount; ++i) {
        parents[i] = Parents[i];
        transforms[i] = Transforms[i];
    }
}


Skeleton::Skeleton(const Skeleton &s) :
    parents(new uint32_t[s.boneCount]),
    transforms(new AnimationTransform[s.boneCount]),
    boneCount(s.boneCount)
{
    for (int i = 0; i < boneCount; ++i) {
        parents[i] = s.parents[i];
        transforms[i] = s.transforms[i];
    }
}


Skeleton::Skeleton(Skeleton &&s) :
    parents(s.parents),
    transforms(s.transforms),
    boneCount(s.boneCount)
{
    s.parents = nullptr;
}


Skeleton& Skeleton::operator= (const Skeleton &s)
{
    parents = new uint32_t[s.boneCount];
    transforms = new AnimationTransform[s.boneCount];
    boneCount = s.boneCount;
    for (int i = 0; i < boneCount; ++i) {
        parents[i] = s.parents[i];
        transforms[i] = s.transforms[i];
    }
    return *this;
}


Skeleton::~Skeleton()
{
    if (parents) delete[] parents;
    if (transforms) delete[] transforms;
}
