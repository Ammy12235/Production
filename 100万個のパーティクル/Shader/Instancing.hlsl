// 0番のテクスチャスロットを使用する
Texture2D g_texture : register(t0);
// 0番のサンプラスロットを使用する
SamplerState g_sampler : register(s0);

// 定数バッファ
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

// 頂点シェーダーの入力パラメータ
struct VS_IN
{
	float3 pos : POSITION;    //頂点座標
	float2 uv : TEXCOORD;      // UV座標

};

struct InstanceType
{
	float3 pos:INSTANCE_POSITION;
	float2 uvOffset:INSTANCE_TEXCOORD;
};

// 頂点シェーダーの出力パラメータ
struct VS_OUT
{
	float4 pos  : SV_POSITION;
	float2 uv : TEXCOORD0;      // UV座標
};


// 頂点シェーダー
VS_OUT VS(VS_IN In, InstanceType instanceData)
{
	VS_OUT Out;

	Out.pos = mul(float4(In.pos + instanceData.pos, 1.0f), World);

	Out.pos.x -= cameraPos.x + g_viewPortWidth / 2;
	Out.pos.y -= cameraPos.y + g_viewPortHeight / 2;
	Out.pos = mul(Out.pos, cameraScl);
	Out.pos = mul(Out.pos, cameraRot);
	Out.pos.x += cameraPos.x + g_viewPortWidth / 2;
	Out.pos.y += cameraPos.y + g_viewPortHeight / 2;

	Out.pos.x = ((Out.pos.x - cameraPos.x) / g_viewPortWidth) * 2 - 1;
	Out.pos.y = 1 - ((Out.pos.y - cameraPos.y) / g_viewPortHeight) * 2;

    Out.uv = In.uv+instanceData.uvOffset;
	return Out;
}

// ピクセルシェーダ
float4 PS(VS_OUT In) : SV_TARGET
{
	// テクスチャから色を取得
	float4 texColor = g_texture.Sample(g_sampler, In.uv);
    texColor.a *= alpha;
	// テクスチャの色を出力
	return texColor;
}