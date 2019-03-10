//  Copyright © 2019 Whelan Callahan. All rights reserved.

#include <metal_stdlib>
#include <simd/simd.h>
#include "ShaderTypes.h"

using namespace metal;

struct VertexOut
{
    float4 position [[position]];
    float3 normal;
    float2 uv;
};


vertex VertexOut VertexFunction(
                                const device Vertex *vin [[buffer(0)]],
                                const constant Uniform &uform [[buffer(1)]],
                                uint vid [[vertex_id]])
{
    VertexOut vout;
    vout.position = uform.projViewModelMatrix * float4(vin[vid].position.xyz, 1.0f);
    vout.normal = normalize(uform.normalMatrix * vin[vid].normal);
    vout.uv = vin[vid].uv;
    vout.uv.y = (vout.uv.y * -1.0f) + 1.0f;
    return vout;
}

constexpr sampler s(coord::normalized,
                    address::repeat,
                    filter::linear);

fragment float4 FragmentFunction(VertexOut vin [[stage_in]], texture2d<float> texture [[texture(0)]])
{
    const float ambientStrength = 0.02f;
    const float3 lightColor = float3(1.0f, 1.0f, 1.0f);
    const float3 lightDir = normalize(float3(-1.0f, 0.0f, 0.0f));
    //const float3 objColor = float3(1.0f, 0.0f, 0.0f);
    
    float3 ambient = lightColor * ambientStrength;
    
    float diffuseStrength = max(1.0f - dot(vin.normal, lightDir), 0.0f);
    float3 diffuse = lightColor * diffuseStrength;
    
    //float3 color = (ambient + diffuse) * objColor;
    float3 color = (ambient + diffuse) * texture.sample(s, vin.uv).rgb;
    return float4(color.xyz, 1.0f);
}
