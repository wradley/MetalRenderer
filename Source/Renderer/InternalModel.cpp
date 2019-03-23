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
    animationStartTime(0)
{}


InternalModel::InternalModel(const InternalModel &m) :
    parent(m.parent),
    animations(new Animation[m.animationCount]),
    animationCount(m.animationCount),
    currAnimation(m.currAnimation),
    animationStartTime(m.animationStartTime),
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
    animationStartTime(m.animationStartTime),
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
    animationStartTime = m.animationStartTime;
    colorTexture = m.colorTexture;
    memcpy(animations, m.animations, sizeof(Animation) * animationCount);
    return *this;
}


InternalModel::~InternalModel()
{
    if (animations) delete[] animations;
}


void InternalModel::calculateBoneMatrices(void *dst, uint64_t time)
{
    Animation &animation = animations[currAnimation];
    BoneData *boneDatas = (BoneData*) dst;
    uint64_t animationDuration = animation.frameToTime(animation.totalFrames - 1);
    
    // reset animations if unplayed or over the duration of the loop
    if (animationStartTime == 0) animationStartTime = time;
    if ((time - animationStartTime) > animationDuration) animationStartTime = time;
    uint64_t elapsedTime = time - animationStartTime;
    
    // find surrounding frames
    uint32_t prevFrameIndex = animation.keyFrameCount - 1;
    for (int i = 0; i < animation.keyFrameCount; ++i) {
        uint64_t frameTime = animation.frameToTime(animation.keyFrames[i].frameNumber);
        if (elapsedTime < frameTime) {
            prevFrameIndex = i - 1;
            break;
        }
    }
    uint32_t nextFrameIndex = prevFrameIndex + 1;
    if (nextFrameIndex == animation.keyFrameCount) nextFrameIndex = 0;
    KeyFrame &prevFrame = animation.keyFrames[prevFrameIndex];
    KeyFrame &nextFrame = animation.keyFrames[nextFrameIndex];

    // calculate interpolation
    uint64_t prevTime = animation.frameToTime(prevFrame.frameNumber);
    uint64_t nextTime = animation.frameToTime(nextFrame.frameNumber);
    uint64_t currTime = elapsedTime;
    if (nextTime < prevTime) nextTime += animationDuration;
    if (currTime < prevTime) currTime += animationDuration;
    nextTime = nextTime - prevTime;
    currTime = currTime - prevTime;
    float interp = (float)currTime / (float)nextTime;
    if (interp > 1.0f) interp = 1.0f;
    interp = (cosf((interp * PI) + PI) + 1) / 2.0f; // cosine interpolation

    // calculate all interpolated bone transforms
    for (int i = 0; i < skeleton.boneCount; ++i)
    {
        AnimationTransform &prevTransform = prevFrame.transforms[i];
        AnimationTransform &nextTransform = nextFrame.transforms[i];

        simd_quatf interpRot = simd_slerp(prevTransform.orientation, nextTransform.orientation, interp);
        simd_float3 interpTrans = simd_mix(prevTransform.position, nextTransform.position, interp);
        simd_float4x4 rotMat = simd_matrix4x4(interpRot);
        simd_float4x4 transMat = Matrix::Translation(interpTrans);
        simd_float4x4 matrix = simd_mul(skeleton.transforms[i].toMat(), simd_mul(transMat, rotMat));

        if (i) boneDatas[i].globalTransform = simd_mul(boneDatas[skeleton.parents[i]].globalTransform, matrix);
        else boneDatas[i].globalTransform = matrix;
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
