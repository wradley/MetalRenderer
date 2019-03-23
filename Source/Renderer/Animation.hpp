//  Copyright © 2019 Whelan Callahan. All rights reserved.
#pragma once
#include <string>
#include <simd/simd.h>
#include "../Math/MatrixOperations.hpp"

class AnimationTransform
{
public:
    AnimationTransform();
    AnimationTransform(const AnimationTransform &t);
    AnimationTransform(AnimationTransform &&t);
    AnimationTransform& operator= (const AnimationTransform &t);
    ~AnimationTransform();
    
    inline simd_float4x4 toMat() { return simd_mul(Matrix::Translation(position), simd_matrix4x4(orientation)); }
    
    simd_quatf orientation;
    simd_float3 position;
};


class KeyFrame
{
public:

    KeyFrame();
    KeyFrame(AnimationTransform *transforms, uint32_t count, uint64_t frameNumber);
    KeyFrame(const KeyFrame &f);
    KeyFrame(KeyFrame &&f);
    KeyFrame& operator= (const KeyFrame &f);
    ~KeyFrame();

    // Each transform should be in the position of the bone it is
    // representing in the skeleton.
    AnimationTransform *transforms;
    uint32_t transformCount;
    uint64_t frameNumber;

};


class Animation
{
public:

    Animation();
    // expects an ordered list of frames (by time they occur)
    Animation(KeyFrame *keyFrames, uint32_t keyFrameCount, uint64_t fps, uint64_t totalFrames, const std::string &name);
    Animation(const Animation &a);
    Animation(Animation &&a);
    Animation& operator= (const Animation &a);
    ~Animation();
    
    inline uint64_t frameToTime(uint32_t frame) { return (uint64_t) (1000.0f * (((float)frame) / ((float)fps))); }

    KeyFrame *keyFrames;
    uint32_t keyFrameCount;
    uint64_t totalFrames;
    uint64_t fps;
    std::string name;
};
