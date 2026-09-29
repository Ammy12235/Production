cbuffer cbBuffer0:register(b0)
{
	float2 viewScale:packoffset(c0.x);
	float blurScale : packoffset(c0.z);

};

Texture2D    ColorTex0 : register(t0);
SamplerState ColorSmp : register(s0);


struct VS_IN
{
	float3 Position : POSITION;
	float2 TexCoord : TEXCOORD;
};

struct VS_OUT
{
	float4 Position : SV_POSITION;
	float2 TexCoord : TEXCOORD;
};

struct VSOutput_DS
{
	float4 Position : SV_POSITION;
	float2 TexCoord : TEXCOORD;
};

VS_OUT VS(VS_IN In)
{
	VS_OUT Out;

	Out.Position = float4(In.Position, 1.0f);
	Out.TexCoord = In.TexCoord;

	return Out;
}

float4 PS(VS_OUT In) : SV_TARGET
{
	float4 Color = 0;
	float N = 0;

	for (int i = -10; i < 10; i++)
	{
		for (int j = -10; j < 10; j++)
		{
			//int j = 0;
			float2 xy = sign(float2(i, j));
			float weight = (abs(xy).x + 1) * (abs(xy).y + 1);
			weight *= sign(saturate((10 * 10 - ((i * i) * 2 + (j * j) * 2))));
			Color += (ColorTex0.Sample(ColorSmp,
				float2(In.TexCoord + (float2(i, j) * blurScale) / viewScale.xy))) * weight;
			N += weight;
		}
	}
	Color /= N;
	Color.a = 1;
	return Color;
}

// ************************************************************
// ダウンサンプリング
// ************************************************************

// 頂点シェーダー
VSOutput_DS VSFunc_DS(VS_IN In)
{
	VSOutput_DS Out;
	Out.Position = float4(In.Position, 1.0f);
	Out.TexCoord = In.TexCoord;
	return Out;
}

float4 PSFunc_DS(VSOutput_DS In) : SV_TARGET
{
   return ColorTex0.Sample(ColorSmp, In.TexCoord);
}