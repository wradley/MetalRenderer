//  Copyright © 2019 Whelan Callahan. All rights reserved.
#include "Animation.hpp"
#include "../Math/QuaternionOperations.hpp"


AnimationTransform::AnimationTransform(simd_float3 pos, simd_quatf rot) :
    rotation(rot),
    position(pos)
{}


AnimationTransform::AnimationTransform(const AnimationTransform &t) :
    rotation(t.rotation),
    position(t.position)
{}


AnimationTransform::AnimationTransform(AnimationTransform &&t) :
    rotation(t.rotation),
    position(t.position)
{}


AnimationTransform& AnimationTransform::operator= (const AnimationTransform &t)
{
    rotation = t.rotation;
    position = t.position;
    return *this;
}


AnimationTransform::~AnimationTransform()
{}


KeyFrame::KeyFrame(const AnimationTransform &at, uint32_t fn) :
    transform(at),
    frameNumber(fn)
{}


KeyFrame::KeyFrame(const KeyFrame &f) :
    transform(f.transform),
    frameNumber(f.frameNumber)
{}


KeyFrame::KeyFrame(KeyFrame &&f) :
    transform(f.transform),
    frameNumber(f.frameNumber)
{}


KeyFrame& KeyFrame::operator= (const KeyFrame &f)
{
    transform = f.transform;
    frameNumber = f.frameNumber;
    return *this;
}


KeyFrame::~KeyFrame()
{}


Channel::Channel() :
    keyFrames(nullptr),
    keyFrameCount(0)
{}


Channel::Channel(const KeyFrame *frames, uint32_t count) :
    keyFrames(new KeyFrame[count]),
    keyFrameCount(count)
{
    for (uint32_t i = 0; i < keyFrameCount; ++i)
        keyFrames[i] = frames[i];
}


Channel::Channel(const Channel &c) :
    keyFrames(new KeyFrame[c.keyFrameCount]),
    keyFrameCount(c.keyFrameCount)
{
    for (uint32_t i = 0; i < keyFrameCount; ++i)
        keyFrames[i] = c.keyFrames[i];
}


Channel::Channel(Channel &&c) :
    keyFrames(c.keyFrames),
    keyFrameCount(c.keyFrameCount)
{
    c.keyFrames = nullptr;
}


Channel& Channel::operator= (const Channel &c)
{
    keyFrames = new KeyFrame[c.keyFrameCount];
    keyFrameCount = c.keyFrameCount;
    
    for (uint32_t i = 0; i < keyFrameCount; ++i)
        keyFrames[i] = c.keyFrames[i];
    
    return *this;
}


Channel::~Channel()
{
    if (keyFrames) delete[] keyFrames;
}


uint32_t Channel::keyFrameBefore(uint32_t frame)
{
    for (uint32_t i = 0; i < keyFrameCount; ++i) {
        if (frame < keyFrames[i].frameNumber) return i - 1;
    }
    
    return keyFrameCount - 1;
}


Animation::Animation() :
    channels(nullptr),
    channelCount(0),
    fps(0),
    totalFrames(0),
    name("")
{}


Animation::Animation(const Channel *Channels,
                     uint32_t ChannelCount,
                     float FPS,
                     uint32_t TotalFrames,
                     const std::string &Name) :
    channels(new Channel[ChannelCount]),
    channelCount(ChannelCount),
    fps(FPS),
    totalFrames(TotalFrames),
    name(Name)
{
    for (uint32_t i = 0; i < channelCount; ++i)
        channels[i] = Channels[i];
}


Animation::Animation(const Animation &a) :
    channels(new Channel[a.channelCount]),
    channelCount(a.channelCount),
    fps(a.fps),
    totalFrames(a.totalFrames),
    name(a.name)
{
    for (uint32_t i = 0; i < channelCount; ++i)
        channels[i] = a.channels[i];
}


Animation::Animation(Animation &&a):
    channels(a.channels),
    channelCount(a.channelCount),
    fps(a.fps),
    totalFrames(a.totalFrames),
    name(a.name)
{
    a.channels = nullptr;
}


Animation& Animation::operator= (const Animation &a)
{
    channels = new Channel[a.channelCount];
    channelCount = a.channelCount;
    fps = a.fps;
    totalFrames = a.totalFrames;
    name = a.name;
    
    for (uint32_t i = 0; i < channelCount; ++i)
        channels[i] = a.channels[i];
    
    return *this;
}


Animation::~Animation()
{
    if (channels) delete[] channels;
}
