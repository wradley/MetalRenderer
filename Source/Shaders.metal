//  Copyright © 2019 Whelan Callahan. All rights reserved.

#include <metal_stdlib>
#include <simd/simd.h>
#include "ShaderTypes.h"

using namespace metal;

struct VertexIn
{
    float3 position         [[attribute(0)]];
    float3 normal           [[attribute(1)]];
    float2 uv               [[attribute(2)]];
    int4   boneIDs          [[attribute(3)]];
    float4 boneWeights      [[attribute(4)]];
};

struct VertexOut
{
    float4 position [[position]];
    float3 normal;
    float2 uv;
};

typedef VertexOut FragIn;


float3x3 To3x3(float4x4 mat)
{
    return float3x3(float3(mat[0]), float3(mat[1]), float3(mat[2]));
}


vertex VertexOut VertexFunction(VertexIn vin [[stage_in]],
                                constant Uniform &uform [[buffer(1)]],
                                constant BoneData *boneDatas [[buffer(2)]])
{
    VertexOut vout;
    
    if (uform.boneCount)
    {
        float4x4 boneTransform = boneDatas[vin.boneIDs[0]].globalTransform * vin.boneWeights[0];
        boneTransform         += boneDatas[vin.boneIDs[1]].globalTransform * vin.boneWeights[1];
        boneTransform         += boneDatas[vin.boneIDs[2]].globalTransform * vin.boneWeights[2];
        boneTransform         += boneDatas[vin.boneIDs[3]].globalTransform * vin.boneWeights[3];
        float4x4 boneInverse   = boneDatas[vin.boneIDs[0]].inverseGlobalTransform * vin.boneWeights[0];
        boneInverse           += boneDatas[vin.boneIDs[1]].inverseGlobalTransform * vin.boneWeights[1];
        boneInverse           += boneDatas[vin.boneIDs[2]].inverseGlobalTransform * vin.boneWeights[2];
        boneInverse           += boneDatas[vin.boneIDs[3]].inverseGlobalTransform * vin.boneWeights[3];
        // ---
        vout.position          = uform.projViewWorldMatrix * boneTransform * boneInverse * float4(vin.position, 1.0f);
        vout.normal            = uform.normalMatrix * To3x3(boneTransform) * To3x3(boneInverse) * vin.normal;
    }
    
    else
    {
        vout.position = uform.projViewWorldMatrix * float4(vin.position, 1.0f);
        vout.normal   = uform.normalMatrix * vin.normal;
    }
    
    vout.uv = vin.uv;
    vout.uv.y = (vout.uv.y * -1.0f) + 1.0f;
    return vout;
}


constexpr sampler s(coord::normalized,
                    address::repeat,
                    filter::linear);


fragment float4 FragmentFunction(FragIn fin [[stage_in]], texture2d<float> texture [[texture(0)]])
{
    const float ambientStrength = 0.02f;
    const float3 lightColor = float3(1.0f, 1.0f, 1.0f);
    const float3 lightDir = normalize(float3(-1.0f, 0.0f, 0.0f));
    //const float3 objColor = float3(1.0f, 0.0f, 0.0f);
    
    float3 ambient = lightColor * ambientStrength;
    
    float diffuseStrength = max(1.0f - dot(fin.normal, lightDir), 0.0f);
    float3 diffuse = lightColor * diffuseStrength;
    
    //float3 color = (ambient + diffuse) * objColor;
    float3 color = (ambient + diffuse) * texture.sample(s, fin.uv).rgb;
    return float4(color.xyz, 1.0f);
}
