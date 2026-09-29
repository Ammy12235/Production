#pragma once

#include "eLayer.h"
#include "Core/Base.h"
#include "Graphics/Renderer/DirectX11/BaseShading.h"
#include "Graphics/Renderer/DirectX11/DrawCommand/eBlendState.h"
#include "Graphics/Shader/ShaderBlob.h"
#include "eBaseShading.h"

enum eDrawCommand
{
	DrawComamnd_None,
	DrawComamnd_Point,
	DrawComamnd_Line,
	DrawComamnd_Rect_Color,
	DrawComamnd_Rect_Texture,
	DrawComamnd_Rect_SRV,
	DrawComamnd_Rect_Texture_Instanced,
	DrawCommand_Cube,
	DrawComamnd_Max,
};
struct Command_2D
{
	eLayer layer;//レイヤー
	eDrawCommand drawCommand;//描画する形
	std::vector<Vertex2D> v;//頂点情報
	int vertexNum = 0;//頂点数
	eBlendState blendState;//ブレンドステート
	int alpha = 0;//０～２５５までのアルファ値
	int texId = -1;//テクスチャID
	ID3D11ShaderResourceView* const* m_pSRV = nullptr;
	ID3D11DepthStencilState* m_pDepthStencilState = nullptr;

	Command_2D() = default;

	Command_2D(const Command_2D&) = default;
	Command_2D& operator=(const Command_2D&) = default;

	Command_2D(Command_2D&&) = default;
	Command_2D& operator=(Command_2D&&) = default;

};

struct Command_2D_Instanced
{
	eLayer layer=eLayer::Layer_0;//レイヤー
	eDrawCommand drawCommand=eDrawCommand::DrawComamnd_Rect_Texture_Instanced;//描画する形
	std::vector<InstanceData2D> v;//頂点情報
	size_t vertexNum=0;
	const InstanceData2D* instancedData;//インスタンスデータの先頭ポインタ
	size_t instanceNum=0;//何個描画するか
	eBlendState blendState;//ブレンドステート
	int alpha=1;//０～２５５までのアルファ値
	int texId=0;//テクスチャID
	ID3D11DepthStencilState* m_pDepthStencilState = nullptr;

	Command_2D_Instanced()=default;

	Command_2D_Instanced(const Command_2D_Instanced&) = default;
	Command_2D_Instanced& operator=(const Command_2D_Instanced&) = default;

	Command_2D_Instanced(Command_2D_Instanced&&) = default;
	Command_2D_Instanced& operator=(Command_2D_Instanced&&) = default;
};

struct Command_3D
{
	eLayer layer;//レイヤー
	eDrawCommand drawCommand;//描画する形
	std::vector<Vertex3D> v;//頂点情報
	int vertexNum;//頂点数
	eBlendState blendState;//ブレンドステート
	int alpha;//０～２５５までのアルファ値
	int texId;//テクスチャID
	ID3D11DepthStencilState* m_pDepthStencilState = nullptr;

	Command_3D() = default;

	Command_3D(const Command_3D&) = default;
	Command_3D& operator=(const Command_3D&) = default;

	Command_3D(Command_3D&&) = default;
	Command_3D& operator=(Command_3D&&) = default;
};


struct shaderPipelineInfo
{
	eDrawCommand drawCommand;
	BASE_VERTEXSHADER vShader;
	BASE_PIXELSHADER pShader;
	BASE_LAYOUT layout;
	D3D_PRIMITIVE_TOPOLOGY topology;
};
