cbuffer cbBuffer0:register(b0)
{
	float hue : packoffset(c0.x);
	float saturate : packoffset(c0.y);
	float value : packoffset(c0.z);

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

	float Y = 0.299 * texColor.r + 0.587 * texColor.g + 0.114 * texColor.b;
	float	I = 0.596 * texColor.r - 0.274 * texColor.g - 0.322 * texColor.b;
	float	Q = 0.211 * texColor.r - 0.522 * texColor.g + 0.311 * texColor.b;

	Y *= value;
	I *= value * saturate;
	Q *= value * saturate * hue;

	texColor.r = Y + 0.956 * I + 0.620 * Q;
	texColor.g = Y - 0.272 * I - 0.647 * Q;
	texColor.b = Y - 1.108 * I + 1.705 * Q;

	texColor.a = 1;

	// テクスチャの色を出力
	return texColor;
}