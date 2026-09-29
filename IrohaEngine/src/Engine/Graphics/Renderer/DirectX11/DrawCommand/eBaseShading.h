#pragma once

//コンテナに格納されるシェーダーやバッファはここで列挙しておく。初期化の順序によるため注意

enum BASE_VERTEXSHADER
{
	VERTEX_SHADER_None = -1,
	VERTEX_SHADER_Local,     //カメラ行列を考慮しない頂点シェーダ
	VERTEX_SHADER_World,     //カメラ行列を考慮した頂点シェーダ
	VERTEX_SHADER_Instancing,//GPUインスタンシング用頂点シェーダ(uvOffset機能付き)
	VERTEX_SHADER_3D,        //3Dの描画を行う頂点シェーダ
	VERTEX_SHADER_Max
};

enum BASE_PIXELSHADER
{
	PIXEL_SHADER_None = -1,
	PIXEL_SHADER_2D_Raw,     //単色で塗りつぶすピクセルシェーダ
	PIXEL_SHADER_2D_Texture,//テクスチャを貼り付けるピクセルシェーダ
	PIXEL_SHADER_2D_Texture_UVOnly,//テクスチャを貼り付ける入力がUVのみのピクセルシェーダ
	PIXEL_SHADER_3D_Color,//3Dで描画するピクセルシェーダ（光源計算が適当に行われておりいい加減）
	PIXEL_SHADER_Max,
};

enum BASE_LAYOUT
{
	LAYOUT_None = -1,
	LAYOUT_2D,          //2Dの描画で用いられる入力レイアウト
	LAYOUT_Instancing2D,//GPUインスタンシングで用いられる入力レイアウト
	LAYOUT_3D,          //3Dの描画で用いられる入力レイアウト
	LAYOUT_Max,
};

enum BASE_VBUFFER
{
	VERTEX_BUFFER_None=-1,
	VERTEX_BUFFER_2D,
	VERTEX_BUFFER_Instancing2D_Original,
	VERTEX_BUFFER_Instancing2D,
	VERTEX_BUFFER_3D,
	VERTEX_BUFFER_Max,
};

enum BASE_CBUFFER
{
	CONSTANT_BUFFER_None=-1,
	CONSTANT_BUFFER_2D,
	CONSTANT_BUFFER_3D,
	CONSTANT_BUFFER_Max,
};

enum BASE_IBUFFER
{
	INDEX_BUFFER_None=-1,
	INDEX_BUFFER_3D,
	INDEX_BUFFER_Max,
};
