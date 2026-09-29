
Texture2D    ColorTex0 : register(t0);
SamplerState ColorSmp : register(s0);


cbuffer ConstantBuffer: register(b0)
{
    float2 resolution;
    float time;
    float alpha;
}



float3x3 rotX(float a) {
    float c = cos(a);
    float s = sin(a);
    return float3x3(
        1, 0, 0,
        0, c, -s,
        0, s, c
    );
}

float3x3 rotY(float a) {
    float c = cos(a);
    float s = sin(a);
    return float3x3(
        c, 0, -s,
        0, 1, 0,
        s, 0, c
    );
}

float random(float2 texCoord) {
    return frac(sin(dot(texCoord.xy, float2(12.9898, 78.233))) * 43758.5453);
}

float noise(float2 texCoord) {
    float2 i = floor(texCoord);
    float2 f = frac(texCoord);
    float a = random(i + float2(0.0, 0.0));
    float b = random(i + float2(1.0, 0.0));
    float c = random(i + float2(0.0, 1.0));
    float d = random(i + float2(1.0, 1.0));
    float2 u = f * f * (3.0 - 2.0 * f);
    return lerp(a, b, u.x) + (c - a) * u.y * (1.0 - u.x) + (d - b) * u.x * u.y;
}

float fbm(float2 texCoord) {
    float v = 0.0;
    float a = 0.5;
    float2 shift = float2(100,0);
    float2x2 rot = float2x2(cos(0.5), sin(0.5), -sin(0.5), cos(0.5));
    for (int i = 0; i < 6; i++) {
        float dir = fmod(float(i), 2.0) > 0.5 ? 1.0 : -1.0;
        v += a * noise(texCoord - 0.05 * dir * time);

        texCoord = mul(texCoord,rot) * 2.0 + shift;
        a *= 0.5;
    }
    return v;
}

struct VS_OUT
{
	float4 Position : SV_POSITION;
	float2 TexCoord : TEXCOORD;
};

float4 PS(VS_OUT In) : SV_TARGET
{
    float2 p = (In.TexCoord.xy * 3.0 - resolution.xy) / min(resolution.x, resolution.y);
    p -= float2(12.0, 0.0);

    float t = 0.0, d;

    float time2 = 1.0;

    float2 q = float2(0,0);
    q.x = fbm(p + 0.00 * time2);
    q.y = fbm(p + float2(1,0));
    float2 r = float2(0,0);
    r.x = fbm(p + 1.0 * q + float2(1.7, 1.2) + 0.15 * time2);
    r.y = fbm(p + 1.0 * q + float2(8.3, 2.8) + 0.126 * time2);
    float f = fbm(p + r);

    // DS: hornidev
    float3 color = lerp(
        float3(1.0, 1.0, 2.0),
        float3(1.0, 1.0, 1.0),
        clamp((f * f) * 5.5, 1.2, 15.5)
    );

    color = lerp(
        color,
        float3(1.0, 1.0, 1.0),
        clamp(length(q), 2.0, 2.0)
    );

    color = lerp(
        color,
        float3(0.3, 0.2, 1.0),
        clamp(length(r.x), 0.0, 5.0)
    );

    color = (f * f * f * 1.0 + 0.5 * 1.7 * 0.0 + 0.9 * f) * color;

    float2 uv = In.TexCoord.xy / resolution.xy;
    float alpha = 50.0 - max(pow(100.0 * distance(uv.x, -1.0), 0.0), pow(2.0 * distance(uv.y, 0.5), 5.0));
    float4 result= float4(color , alpha * color.r);
	// èoóÕ
	return result;
}


