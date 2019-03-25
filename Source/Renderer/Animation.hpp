//  Copyright © 2019 Whelan Callahan. All rights reserved.
#pragma once
#include <string>
#include <simd/simd.h>
#include "../Math/MatrixOperations.hpp"

class AnimationTransform
{
public:
    AnimationTransform(simd_float3 position = simd_make_float3(0, 0, 0), simd_quatf rotation = simd_quaternion(0.0f, 0.0f, 0.0f, 1.0f));
    AnimationTransform(const AnimationTransform &t);
    AnimationTransform(AnimationTransform &&t);
    AnimationTransform& operator= (const AnimationTransform &t);
    ~AnimationTransform();
    
    inline simd_float4x4 toMat() {
        return simd_mul(Matrix::Translation(position), simd_matrix4x4(rotation));
    }
    
    simd_quatf rotation;
    simd_float3 position;
};


class KeyFrame
{
public:

    KeyFrame(const AnimationTransform &at = AnimationTransform(), uint32_t fn = 0);
    KeyFrame(const KeyFrame &f);
    KeyFrame(KeyFrame &&f);
    KeyFrame& operator= (const KeyFrame &f);
    ~KeyFrame();

    AnimationTransform transform;
    uint32_t frameNumber;
    
};


class Channel
{
public:
    
    // expects key frames to be ordered by frame number
    Channel();
    Channel(const KeyFrame *frames, uint32_t count);
    Channel(const Channel &c);
    Channel(Channel &&c);
    Channel& operator= (const Channel &c);
    ~Channel();
    
    uint32_t keyFrameBefore(uint32_t frame);

    KeyFrame *keyFrames;
    uint32_t keyFrameCount;
    
};


class Animation
{
public:

    Animation();
    Animation(const Channel *channels,
              uint32_t channelCount,
              float fps,
              uint32_t totalFrames,
              const std::string &name);
    Animation(const Animation &a);
    Animation(Animation &&a);
    Animation& operator= (const Animation &a);
    ~Animation();

    // one channel per bone
    Channel *channels;
    uint32_t channelCount;
    
    float fps;
    uint32_t totalFrames;
    
    std::string name;
};
