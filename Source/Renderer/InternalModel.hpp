//  Copyright © 2019 Whelan Callahan. All rights reserved.
#pragma once
#include <simd/simd.h>
#include "Skeleton.hpp"
#include "Animation.hpp"
#include "Mesh.hpp"

class InternalModel
{
public:

    InternalModel(uint64_t parent = 0);
    InternalModel(const InternalModel &m);
    InternalModel(InternalModel &&m);
    InternalModel& operator= (const InternalModel &m);
    ~InternalModel();

    void calculateBoneMatrices(void *dst, uint64_t time);
    void calculateBoneInverseMatrices(void *dst);

public:

    uint64_t parent;
    
    Animation *animations;
    uint32_t animationCount;
    uint32_t currAnimation;
    uint64_t animationStartTime; // start time of loop in animation
    
    Skeleton skeleton;
    Mesh mesh;

    uint64_t colorTexture;

};
