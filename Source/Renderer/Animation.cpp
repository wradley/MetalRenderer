//  Copyright © 2019 Whelan Callahan. All rights reserved.
#include "Animation.hpp"
#include "../Math/QuaternionOperations.hpp"


AnimationTransform::AnimationTransform() :
    orientation(Quat::FromAxisAngle(simd_make_float3(0, 1, 0), 0)),
    position(simd_make_float3(0, 0, 0))
{}


AnimationTransform::AnimationTransform(const AnimationTransform &t) :
    orientation(t.orientation),
    position(t.position)
{}


AnimationTransform::AnimationTransform(AnimationTransform &&t) :
    orientation(t.orientation),
    position(t.position)
{}


AnimationTransform& AnimationTransform::operator= (const AnimationTransform &t)
{
    orientation = t.orientation;
    position = t.position;
    return *this;
}


AnimationTransform::~AnimationTransform()
{}


KeyFrame::KeyFrame() :
    transforms(nullptr),
    transformCount(0),
    frameNumber(0)
{}


KeyFrame::KeyFrame(AnimationTransform *Transforms, uint32_t Count, uint64_t FrameNumber) :
    transforms(new AnimationTransform[Count]),
    transformCount(Count),
    frameNumber(FrameNumber)
{
    for (int i = 0; i < transformCount; ++i) {
        transforms[i] = Transforms[i];
    }
}


KeyFrame::KeyFrame(const KeyFrame &f) :
    transforms(new AnimationTransform[f.transformCount]),
    transformCount(f.transformCount),
    frameNumber(f.frameNumber)
{
    for (int i = 0; i < transformCount; ++i) {
        transforms[i] = f.transforms[i];
    }
}


KeyFrame::KeyFrame(KeyFrame &&f) :
    transforms(f.transforms),
    transformCount(f.transformCount),
    frameNumber(f.frameNumber)
{
    f.transforms = nullptr;
}


KeyFrame& KeyFrame::operator= (const KeyFrame &f)
{
    transforms = new AnimationTransform[f.transformCount];
    transformCount = f.transformCount;
    frameNumber = f.frameNumber;
    for (int i = 0; i < transformCount; ++i) {
        transforms[i] = f.transforms[i];
    }
    return *this;
}


KeyFrame::~KeyFrame()
{
    if (transforms) delete[] transforms;
}


Animation::Animation() :
    keyFrames(nullptr),
    keyFrameCount(0),
    totalFrames(0),
    fps(60),
    name("")
{}


Animation::Animation(KeyFrame *KeyFrames, uint32_t KeyFrameCount, uint64_t FPS, uint64_t TotalFrames, const std::string &Name) :
    keyFrames(new KeyFrame[KeyFrameCount]),
    keyFrameCount(KeyFrameCount),
    totalFrames(TotalFrames),
    fps(FPS),
    name(Name)
{
    for (int i = 0; i < keyFrameCount; ++i) {
        keyFrames[i] = KeyFrames[i];
    }
}


Animation::Animation(const Animation &a) :
    keyFrames(new KeyFrame[a.keyFrameCount]),
    keyFrameCount(a.keyFrameCount),
    totalFrames(a.totalFrames),
    fps(a.fps),
    name(a.name)
{
    for (int i = 0; i < keyFrameCount; ++i) {
        keyFrames[i] = a.keyFrames[i];
    }
}


Animation::Animation(Animation &&a) :
    keyFrames(a.keyFrames),
    keyFrameCount(a.keyFrameCount),
    totalFrames(a.totalFrames),
    fps(a.fps),
    name(a.name)
{
    a.keyFrames = nullptr;
}


Animation& Animation::operator= (const Animation &a)
{
    keyFrames = new KeyFrame[a.keyFrameCount];
    keyFrameCount = a.keyFrameCount;
    totalFrames = a.totalFrames;
    fps = a.fps;
    name = a.name;
    for (int i = 0; i < keyFrameCount; ++i) {
        keyFrames[i] = a.keyFrames[i];
    }
    return *this;
}


Animation::~Animation()
{
    if (keyFrames) delete[] keyFrames;
}
