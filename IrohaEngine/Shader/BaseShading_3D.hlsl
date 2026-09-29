struct VS_IN
{
    float4 pos : POSITION0;
    float4 col:COLOR0;
    float4 nor : NORMAL0;
};

struct VS_OUT
{
    float4 pos  : SV_POSITION;
    float4 posw : POSITION0;
    float4 col: COLOR0;
    float4 norw : NORMAL0;
};

struct PS_IN
{
    float4 pos  : SV_POSITION;
    float4 posw : POSITION0;
    float4 col: COLOR;
    float4 norw : NORMAL0;
};

cbuffer ConstantBuffer
{
    float4x4 World;         //ワールド変換行列
    float4x4 View;          //ビュー変換行列
    float4x4 Projection;    //透視射影変換行列
    float4   Light;         //光源座標
    float4   LightColor;
    float4   Attenuation;   //光源減衰パラメータ
}

VS_OUT VS(VS_IN input)//3D
{
    VS_OUT output;
    float3 nor;
    float  col;

    output.posw = mul(input.pos, World);
    output.pos = mul(output.posw, View);
    output.pos = mul(output.pos, Projection);

    output.norw = mul(input.nor, World);

    output.col = input.col;

    return output;
}


//--------------------------------------------------------------------------------------
// ピクセルシェーダ
//--------------------------------------------------------------------------------------
float4 PS(PS_IN input) : SV_Target//単色のレンダリング
{
    float3 dir;
    float  len;
    float  colD;
    float  colA;
    float  col;

    //点光源の方向
    dir = Light.xyz - input.posw.xyz;

    //点光源の距離
    len = length(dir);

    //点光源の方向をnormalize
    dir = dir / len;

    //拡散
    colD = saturate(dot(normalize(input.norw.xyz), dir));
    //減衰
    colA = saturate(1.0f / (Attenuation.x + Attenuation.y * len + Attenuation.z * len * len));

    col = colD * colA+0.2f;
    return float4(LightColor.w* input.col.xyz * col * LightColor.xyz, 1.0f);
}