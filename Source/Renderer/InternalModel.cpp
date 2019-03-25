//  Copyright © 2019 Whelan Callahan. All rights reserved.
#include "InternalModel.hpp"
#include "../ShaderTypes.h"
#include "../Math/MatrixOperations.hpp"
#include "../Math/MathDefines.h"

InternalModel::InternalModel(uint64_t Parent) :
    parent(Parent),
    animations(nullptr),
    animationCount(0),
    currAnimation(0),
    animationLoopStartTime(0)
{}


InternalModel::InternalModel(const InternalModel &m) :
    parent(m.parent),
    animations(new Animation[m.animationCount]),
    animationCount(m.animationCount),
    currAnimation(m.currAnimation),
    animationLoopStartTime(m.animationLoopStartTime),
    skeleton(m.skeleton),
    mesh(m.mesh),
    colorTexture(m.colorTexture)
{
    memcpy(animations, m.animations, sizeof(Animation) * animationCount);
}


InternalModel::InternalModel(InternalModel &&m) :
    parent(m.parent),
    animations(m.animations),
    animationCount(m.animationCount),
    currAnimation(m.currAnimation),
    animationLoopStartTime(m.animationLoopStartTime),
    skeleton(std::move(m.skeleton)),
    mesh(m.mesh),
    colorTexture(m.colorTexture)
{
    m.animations = nullptr;
}


InternalModel& InternalModel::operator= (const InternalModel &m)
{
    parent = m.parent;
    skeleton = m.skeleton;
    mesh = m.mesh;
    animations = new Animation[m.animationCount];
    animationCount = m.animationCount;
    currAnimation = m.currAnimation;
    animationLoopStartTime = m.animationLoopStartTime;
    colorTexture = m.colorTexture;
    memcpy(animations, m.animations, sizeof(Animation) * animationCount);
    return *this;
}


InternalModel::~InternalModel()
{
    if (animations) delete[] animations;
}

void InternalModel::calculateBoneMatrices(void *dst, double time)
{
    Animation &animation = animations[currAnimation];
    BoneData *boneDatas = reinterpret_cast<BoneData*>(dst);
    double totalLoopTime = (double)animation.totalFrames / animation.fps;
    if (animationLoopStartTime == 0) animationLoopStartTime = time;
    if (time > (animationLoopStartTime + totalLoopTime))
        animationLoopStartTime = time;
    time -= animationLoopStartTime;
    uint32_t frame = (uint32_t) (time * (double)animation.fps);

    // iterate all bones from parents down to child bones
    for (uint32_t boneIndex = 0; boneIndex < animation.channelCount; ++boneIndex)
    {
        // calculate interpolation
        Channel &channel = animation.channels[boneIndex];
        uint32_t prevIndex = channel.keyFrameBefore(frame);
        uint32_t nextIndex = (prevIndex == (channel.keyFrameCount-1)) ? prevIndex : prevIndex + 1;
        double interp;
        if (prevIndex == nextIndex)
            interp = 0.0;
        else {
            double prevKeyTime = ((double)channel.keyFrames[prevIndex].frameNumber) / animation.fps;
            double currKeyTime = time;
            double nextKeyTime = ((double)channel.keyFrames[nextIndex].frameNumber) / animation.fps;
            if (currKeyTime < prevKeyTime) currKeyTime += totalLoopTime;
            if (nextKeyTime < prevKeyTime) nextKeyTime += totalLoopTime;
            currKeyTime -= prevKeyTime;
            nextKeyTime -= prevKeyTime;
            interp = currKeyTime / nextKeyTime;
        }
//        if (nextKeyTime == 0) interp = 1;
//        else interp = currKeyTime / nextKeyTime;
        
        // calculate interpolated transform
        AnimationTransform &prevTform = channel.keyFrames[prevIndex].transform;
        AnimationTransform &nextTform = channel.keyFrames[nextIndex].transform;
        simd_float3 interpPos = simd_mix(prevTform.position, nextTform.position, interp);
        simd_quatf interpRot = simd_slerp(prevTform.rotation, nextTform.rotation, interp);
        simd_float4x4 localAnim = simd_mul(Matrix::Translation(interpPos), simd_matrix4x4(interpRot));
        simd_float4x4 localMat = simd_mul(skeleton.transforms[boneIndex].toMat(), localAnim);
        
        // if this bone is not the root
        if (boneIndex) {
            uint32_t parent = skeleton.parents[boneIndex];
            boneDatas[boneIndex].globalTransform = simd_mul(boneDatas[parent].globalTransform, localMat);
        }
        
        // if this is the root bone
        else boneDatas[boneIndex].globalTransform = localMat;
    }
}


void InternalModel::calculateBoneInverseMatrices(void *dst)
{
    BoneData *boneDatas = (BoneData*) dst;
    simd_float4x4 *matrices = new simd_float4x4[skeleton.boneCount];

    for (int i = 0; i < skeleton.boneCount; ++i)
    {
        if (i)
            matrices[i] = simd_mul(matrices[skeleton.parents[i]], skeleton.transforms[i].toMat());
        else // root bone
            matrices[i] = skeleton.transforms[i].toMat();

        boneDatas[i].inverseGlobalTransform = simd_inverse(matrices[i]);
    }
}
