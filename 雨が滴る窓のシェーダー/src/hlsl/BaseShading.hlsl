// 1番のテクスチャスロットを使用する
Texture2D g_texture : register(t0);
// 1番のサンプラスロットを使用する
SamplerState g_sampler : register(s0);

struct VS_IN
{
	float3 pos : POSITION;    //頂点座標
	float4 col:COLOR;
	float2 uv : TEXUV;      // UV座標

};

struct VS_OUT
{
	float4 pos  : SV_POSITION;
	float4 col:COLOR;
	float2 uv : TEXCOORD;      // UV座標
};


struct PS_IN
{
	float4 pos  : SV_POSITION;
	float4 col:COLOR0;
	float2 uv : TEXCOORD0; // UV座標
};

cbuffer ConstantBuffer: register(b0)
{
	column_major float4x4 World:packoffset(c0);         //ワールド変換行列
	float alpha : packoffset(c4.x);
	unsigned int g_viewPortWidth : packoffset(c4.y);
	unsigned int g_viewPortHeight : packoffset(c4.z);
}

cbuffer CameraConstantBuffer: register(b1)
{
	column_major float4x4 cameraRot : packoffset(c0);//カメラ情報
	column_major float4x4 cameraScl:packoffset(c4);
	float2 cameraPos:packoffset(c8);
}
//--------------------------------------------------------------------------------------
// 頂点シェーダ
//--------------------------------------------------------------------------------------

VS_OUT VS(VS_IN input)
{
	VS_OUT output;

	output.pos = mul(float4(input.pos, 1.0f), World);
	
	output.pos.x -= cameraPos.x+g_viewPortWidth / 2;
	output.pos.y -= cameraPos.y+g_viewPortHeight / 2;
	output.pos = mul(output.pos, cameraScl);
	output.pos = mul(output.pos, cameraRot);
	output.pos.x += cameraPos.x + g_viewPortWidth / 2;
	output.pos.y += cameraPos.y + g_viewPortHeight / 2;
	
	output.pos.x = ((output.pos.x - cameraPos.x) / g_viewPortWidth) * 2 - 1;
	output.pos.y = 1 - ((output.pos.y - cameraPos.y) / g_viewPortHeight) * 2;

	output.col = input.col;
	output.uv = input.uv;


	return output;
}



//--------------------------------------------------------------------------------------
// ピクセルシェーダ
//--------------------------------------------------------------------------------------

float4 PS(VS_OUT input) : SV_Target//単色のレンダリング
{
	float4 Col = input.col;

	Col.a *= alpha;

	return Col;
}

float4 PS2(VS_OUT input) : SV_Target//テクスチャ付きのレンダリング
{

	// テクスチャから色を取得
	float4 texColor = g_texture.Sample(g_sampler, input.uv);
	
	float Y = 0.299 * texColor.r + 0.587 * texColor.g + 0.114 * texColor.b;
	float	I = 0.596 * texColor.r - 0.274 * texColor.g - 0.322 * texColor.b;
	float	Q = 0.211 * texColor.r - 0.522 * texColor.g + 0.311 * texColor.b;

	Y *= 1; I *= 0.7; Q *= 0.7;

	texColor.r = Y + 0.956 * I + 0.620 * Q;
	texColor.g = Y - 0.272 * I - 0.647 * Q;
	texColor.b = Y - 1.108 * I + 1.705 * Q;
	

	texColor.a *= alpha;

	// テクスチャの色を出力
	return texColor;
	
}
