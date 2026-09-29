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

VS_OUT VS(VS_IN In)
{
	VS_OUT Out;

	Out.Position = float4(In.Position, 1.0f);
	Out.TexCoord = In.TexCoord;

	return Out;
}

float4 PS(VS_OUT In) : SV_TARGET
{
	// テクスチャから色を取得
	float4 texColor = ColorTex0.Sample(ColorSmp, In.TexCoord);

	texColor.a = 1;

	// テクスチャの色を出力
	return texColor;
}