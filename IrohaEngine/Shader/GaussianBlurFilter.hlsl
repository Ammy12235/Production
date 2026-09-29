// 定数バッファ０
cbuffer cbBuffer0 : register(b0)
{
    float4  Weights0  : packoffset(c0);
    float4  Weights1  : packoffset(c1);
    float   OffsetX : packoffset(c2.x);
    float   OffsetY : packoffset(c2.y);
    float   w : packoffset(c2.z);
    float   h : packoffset(c2.w);
};

Texture2D    ColorTex : register(t0);
SamplerState ColorSmp : register(s0);

struct VSInput
{
    float3 Position : POSITION;
    float2 TexCoord : TEXCOORD;
};

struct VSOutput_DS
{
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD;
};

/////////////////////////////////////////////////////////////////////////
// VSOutput structure
/////////////////////////////////////////////////////////////////////////
struct VSOutput
{
    float4 Position  : SV_POSITION;
    float2 TexCoord0 : TEXCOORD0;
    float2 TexCoord1 : TEXCOORD1;
    float2 TexCoord2 : TEXCOORD2;
    float2 TexCoord3 : TEXCOORD3;
    float2 TexCoord4 : TEXCOORD4;
    float2 TexCoord5 : TEXCOORD5;
    float2 TexCoord6 : TEXCOORD6;
    float2 TexCoord7 : TEXCOORD7;
};

//-----------------------------------------------------------------------
//! @brief 横方向にずらすテクスチャ座標を計算
//-----------------------------------------------------------------------
VSOutput VSFunc_Pass1(VSInput input)
{
    VSOutput output = (VSOutput)0;

    output.Position = float4(input.Position, 1.0f);

    output.TexCoord0 = input.TexCoord + float2(-1.0f / w, 0.0f);
    output.TexCoord1 = input.TexCoord + float2(-3.0f / w, 0.0f);
    output.TexCoord2 = input.TexCoord + float2(-5.0f / w, 0.0f);
    output.TexCoord3 = input.TexCoord + float2(-7.0f / w, 0.0f);
    output.TexCoord4 = input.TexCoord + float2(-9.0f / w, 0.0f);
    output.TexCoord5 = input.TexCoord + float2(-11.0f / w, 0.0f);
    output.TexCoord6 = input.TexCoord + float2(-13.0f / w, 0.0f);
    output.TexCoord7 = input.TexCoord + float2(-15.0f / w, 0.0f);

    return output;
}

//-----------------------------------------------------------------------
//! @brief 縦方向にずらすテクスチャ座標を計算
//-----------------------------------------------------------------------
VSOutput VSFunc_Pass2(VSInput input)
{
    VSOutput output = (VSOutput)0;

    output.Position = float4(input.Position, 1.0f);

    output.TexCoord0 = input.TexCoord + float2(0.0f, -1.0f / h);
    output.TexCoord1 = input.TexCoord + float2(0.0f, -3.0f / h);
    output.TexCoord2 = input.TexCoord + float2(0.0f, -5.0f / h);
    output.TexCoord3 = input.TexCoord + float2(0.0f, -7.0f / h);
    output.TexCoord4 = input.TexCoord + float2(0.0f, -9.0f / h);
    output.TexCoord5 = input.TexCoord + float2(0.0f, -11.0f / h);
    output.TexCoord6 = input.TexCoord + float2(0.0f, -13.0f / h);
    output.TexCoord7 = input.TexCoord + float2(0.0f, -15.0f / h);

    return output;
}

//-----------------------------------------------------------------------
//! @brief 横方向にぼかす
//-----------------------------------------------------------------------
float4 PSFunc_Pass1(VSOutput input) : SV_TARGET
{
    float4 output = float4(0.0f, 0.0f, 0.0f, 0.0f);

    output += Weights0.x * (ColorTex.Sample(ColorSmp, input.TexCoord0) + ColorTex.Sample(ColorSmp, input.TexCoord7 + float2(OffsetX, 0.0f)));
    output += Weights0.y * (ColorTex.Sample(ColorSmp, input.TexCoord1) + ColorTex.Sample(ColorSmp, input.TexCoord6 + float2(OffsetX, 0.0f)));
    output += Weights0.z * (ColorTex.Sample(ColorSmp, input.TexCoord2) + ColorTex.Sample(ColorSmp, input.TexCoord5 + float2(OffsetX, 0.0f)));
    output += Weights0.w * (ColorTex.Sample(ColorSmp, input.TexCoord3) + ColorTex.Sample(ColorSmp, input.TexCoord4 + float2(OffsetX, 0.0f)));

    output += Weights1.x * (ColorTex.Sample(ColorSmp, input.TexCoord4) + ColorTex.Sample(ColorSmp, input.TexCoord3 + float2(OffsetX, 0.0f)));
    output += Weights1.y * (ColorTex.Sample(ColorSmp, input.TexCoord5) + ColorTex.Sample(ColorSmp, input.TexCoord2 + float2(OffsetX, 0.0f)));
    output += Weights1.z * (ColorTex.Sample(ColorSmp, input.TexCoord6) + ColorTex.Sample(ColorSmp, input.TexCoord1 + float2(OffsetX, 0.0f)));
    output += Weights1.w * (ColorTex.Sample(ColorSmp, input.TexCoord7) + ColorTex.Sample(ColorSmp, input.TexCoord0 + float2(OffsetX, 0.0f)));

    return output;
}


//-----------------------------------------------------------------------
//! @brief 縦方向にぼかす
//-----------------------------------------------------------------------
float4 PSFunc_Pass2(VSOutput input) : SV_TARGET
{
    float4 output = float4(0.0f, 0.0f, 0.0f, 0.0f);

    output += Weights0.x * (ColorTex.Sample(ColorSmp, input.TexCoord0) + ColorTex.Sample(ColorSmp, input.TexCoord7 + float2(0.0f, OffsetY)));
    output += Weights0.y * (ColorTex.Sample(ColorSmp, input.TexCoord1) + ColorTex.Sample(ColorSmp, input.TexCoord6 + float2(0.0f, OffsetY)));
    output += Weights0.z * (ColorTex.Sample(ColorSmp, input.TexCoord2) + ColorTex.Sample(ColorSmp, input.TexCoord5 + float2(0.0f, OffsetY)));
    output += Weights0.w * (ColorTex.Sample(ColorSmp, input.TexCoord3) + ColorTex.Sample(ColorSmp, input.TexCoord4 + float2(0.0f, OffsetY)));

    output += Weights1.x * (ColorTex.Sample(ColorSmp, input.TexCoord4) + ColorTex.Sample(ColorSmp, input.TexCoord3 + float2(0.0f, OffsetY)));
    output += Weights1.y * (ColorTex.Sample(ColorSmp, input.TexCoord5) + ColorTex.Sample(ColorSmp, input.TexCoord2 + float2(0.0f, OffsetY)));
    output += Weights1.z * (ColorTex.Sample(ColorSmp, input.TexCoord6) + ColorTex.Sample(ColorSmp, input.TexCoord1 + float2(0.0f, OffsetY)));
    output += Weights1.w * (ColorTex.Sample(ColorSmp, input.TexCoord7) + ColorTex.Sample(ColorSmp, input.TexCoord0 + float2(0.0f, OffsetY)));

    return output;
}

// ************************************************************
// ダウンサンプリング
// ************************************************************

// 頂点シェーダー
VSOutput_DS VSFunc_DS(VSInput In)
{
    VSOutput_DS Out;
    Out.Position = float4(In.Position, 1.0f);
    Out.TexCoord = In.TexCoord;
    return Out;
}

float4 PSFunc_DS(VSOutput_DS In) : SV_TARGET
{
   return ColorTex.Sample(ColorSmp, In.TexCoord);
}