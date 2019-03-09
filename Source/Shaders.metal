//  Copyright © 2019 Whelan Callahan. All rights reserved.

#include <metal_stdlib>
#include <simd/simd.h>
#include "ShaderTypes.h"

using namespace metal;

struct VertexOut
{
    float4 position [[position]];
    float3 normal;
};


vertex VertexOut VertexFunction(
                                const device Vertex *vin [[buffer(0)]],
                                const constant Uniform &uform [[buffer(1)]],
                                uint vid [[vertex_id]])
{
    VertexOut vout;
    vout.position = uform.projViewModelMatrix * float4(vin[vid].position.xyz, 1.0f);
    vout.normal = normalize(uform.normalMatrix * vin[vid].normal);
    return vout;
}


fragment float4 FragmentFunction(VertexOut vin [[stage_in]])
{
    const float ambientStrength = 0.2f;
    const float3 lightColor = float3(1.0f, 1.0f, 1.0f);
    const float3 lightDir = normalize(float3(1.0f, 1.0f, 1.0f));
    const float3 objColor = float3(1.0f, 0.0f, 0.0f);
    
    float3 ambient = lightColor * ambientStrength;
    
    float diffuseStrength = max(dot(vin.normal, lightDir), 0.0f);
    float3 diffuse = lightColor * diffuseStrength;
    
    float3 color = (ambient + diffuse) * objColor;
    return float4(color.xyz, 1.0f);
}
