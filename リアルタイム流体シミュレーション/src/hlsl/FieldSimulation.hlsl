
Texture2D<float4> _SourceVelocity : register(t0);
Texture2D<float4> _SourcePressure : register(t1);
Texture2D<float4> _SourceTexture : register(t2);

RWTexture2D<float4> _ResultVelocity : register(u0);
RWTexture2D<float4> _ResultPressure : register(u1);
RWTexture2D<float4> _ResultDivergence : register(u2);
RWTexture2D<float4> _ResultTexture : register(u3);

SamplerState _LinearClamp : register(s0);

struct ExternalForce
{
    float2 position;
    float2 velocity; // パディング
};

cbuffer ExternalForces : register(b1)
{
    ExternalForce externalForce[16];
};

cbuffer ConstantBuffer : register(b0)
{
    float _DeltaTime : packoffset(c0.x);
    float _Scale : packoffset(c0.y);
    float _Width : packoffset(c0.z);
    float _Height : packoffset(c0.w);
    float _Attenuation : packoffset(c1.x);
}



[numthreads(16, 16, 1)]
void UpdateAdvection(uint2 id : SV_DispatchThreadID)
{

    float w = _Width;
    float h = _Height;

    float3 px = float3(1.0 / w, 1.0 / h, 0.0);
    float2 uv = float2(id.x / w, id.y / h) + px.xy * 0.5;

    float2 velocity = _SourceVelocity.SampleLevel(_LinearClamp, uv, 0).xy;
    float2 result = _SourceVelocity.SampleLevel(_LinearClamp, uv - velocity * _DeltaTime, 0).xy;

    _ResultVelocity[id] = float4(result, 0.0, 1.0);
}

[numthreads(16, 16, 1)]
void InteractionForce(uint2 id : SV_DispatchThreadID)
{
    float w = _Width;
    float h = _Height;

    float2 px = float2(1.0 / w, 1.0 / h);
    float2 uv = float2(id.x / w, id.y / h) + px.xy * 0.5;

    float3 vec = _SourceVelocity.SampleLevel(_LinearClamp, uv, 0).xyz;

    for (int i = 0; i < 16; i++)
    {
       
        float dist = distance(externalForce[i].position * px * _Scale, uv);

        if (dist < 0.001)
        {
            vec.xy += externalForce[i].velocity;
        }
    }
    
  
    
    _ResultVelocity[id] = float4(vec, 1.0);
}

[numthreads(16, 16, 1)]
void UpdateDivergence(uint2 id : SV_DispatchThreadID)
{
    float w = _Width;
    float h = _Height;

    float3 px = float3(1.0 / w, 1.0 / h, 0);
    float2 uv = float2(id.x / w, id.y / h) + px.xy * 0.5;

    float x0 = _SourceVelocity.SampleLevel(_LinearClamp, uv - px.xz, 0).x;
    float x1 = _SourceVelocity.SampleLevel(_LinearClamp, uv + px.xz, 0).x;
    float y0 = _SourceVelocity.SampleLevel(_LinearClamp, uv - px.zy, 0).y;
    float y1 = _SourceVelocity.SampleLevel(_LinearClamp, uv + px.zy, 0).y;

    float divergence = (x1 - x0 + y1 - y0);

    _ResultDivergence[id] = float4(divergence.xx, 0.0, 1.0);
}


[numthreads(16, 16, 1)]
void UpdatePressure(uint2 id : SV_DispatchThreadID)
{
    float w = _Width;
    float h = _Height;

    float3 px = float3(1.0 / w, 1.0 / h, 0);
    float2 uv = float2(id.x / w, id.y / h) + px.xy * 0.5;

    float x0 = _SourcePressure.SampleLevel(_LinearClamp, uv - px.xz, 0).r;
    float x1 = _SourcePressure.SampleLevel(_LinearClamp, uv + px.xz, 0).r;
    float y0 = _SourcePressure.SampleLevel(_LinearClamp, uv - px.zy, 0).r;
    float y1 = _SourcePressure.SampleLevel(_LinearClamp, uv + px.zy, 0).r;

    float d = _ResultDivergence[id].r;
    float relaxed = (x0 + x1 + y0 + y1 -d) * 0.25;
    
    _ResultPressure[id] = float4(relaxed.xx, 0.0, 1.0);
}


[numthreads(16, 16, 1)]
void UpdateVelocity(uint2 id : SV_DispatchThreadID)
{
    float w = _Width;
    float h = _Height;

    float3 px = float3(1.0 / w, 1.0 / h, 0);
    float2 uv = float2(id.x / w, id.y / h) + px.xy * 0.5;

    float x0 = _SourcePressure.SampleLevel(_LinearClamp, uv - px.xz, 0).r;
    float x1 = _SourcePressure.SampleLevel(_LinearClamp, uv + px.xz, 0).r;
    float y0 = _SourcePressure.SampleLevel(_LinearClamp, uv - px.zy, 0).r;
    float y1 = _SourcePressure.SampleLevel(_LinearClamp, uv + px.zy, 0).r;

    float2 v = _SourceVelocity.SampleLevel(_LinearClamp, uv, 0).xy;
    float4 v2 = float4((v - (float2(x1, y1) - float2(x0, y0)) * 0.5), 0.0, 1.0);
    v2 *= _Attenuation;


    _ResultVelocity[id] = v2;
}

[numthreads(16, 16, 1)]
void UpdateTexture(uint2 id : SV_DispatchThreadID)
{
    float w = _Width;
    float h = _Height;

    float3 px = float3(1.0 / w, 1.0 / h, 0);
    float2 uv = float2(id.x / w, id.y / h) + px.xy * 0.5;

    float2 vel = _SourceVelocity.SampleLevel(_LinearClamp, uv, 0).xy;

    float4 col = _SourceTexture.SampleLevel(_LinearClamp, uv - vel * _DeltaTime, 0);

    float vv = saturate(length(vel));
    _ResultTexture[id] = float4(vv, vv, vv, vv);
}
