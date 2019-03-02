//  Copyright © 2019 Whelan Callahan. All rights reserved.

#include <metal_stdlib>
#include <simd/simd.h>
#include "ShaderTypes.h"

struct VertexOut
{
    float4 position [[position]];
    float4 color;
};


vertex VertexOut VertexFunction(const device Vertex *vin [[buffer(0)]], uint vid [[vertex_id]])
{
    VertexOut vout;
    vout.position = float4(vin[vid].position.xy, 0.0f, 1.0f);
    vout.color = vin[vid].color;
    return vout;
}


fragment float4 FragmentFunction(VertexOut vin [[stage_in]])
{
    return vin.color;
}
