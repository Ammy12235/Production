// 0番のテクスチャスロットを使用する
Texture2D g_texture : register(t0);
// 0番のサンプラスロットを使用する
SamplerState g_sampler : register(s0);

struct VS_IN
{
	float4 pos : POSITION0;    //頂点座標
	float4 col:COLOR0;
	float2 uv : TEXUV;      // UV座標

};

struct VS_OUT
{
	float4 pos  : SV_POSITION;
	float4 col:COLOR0;
	float2 uv : TEXCOORD0;      // UV座標
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
	unsigned int g_viewPortWidth : packoffset(c4);
	unsigned int g_viewPortHeight : packoffset(c5);
	float alpha : packoffset(c6);
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

	output.pos = mul(input.pos, World);
	
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

	//グレースケール
	/*
	float Y = texColor.r * 0.29891f + texColor.g * 0.58661f + texColor.b * 0.11448f;
	texColor.r = Y;
	texColor.g = Y;
	texColor.b = Y;
	*/

	texColor.a *= alpha;

	// テクスチャの色を出力
	return texColor;
}
