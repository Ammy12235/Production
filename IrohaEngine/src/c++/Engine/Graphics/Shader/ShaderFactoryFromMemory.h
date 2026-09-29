#pragma once
#include "Core/Base.h"

#include "ShaderBlob.h"
#include "ShaderUtility.h"
class ShaderFactoryFromMemory
{
private:
	//csoファイルの読み込み
	BOOL ReadShader(const std::string& filename, tBlob* sObj);
public:
	//シェーダをcsoファイルから作成する
	bool CreateVertexShaderFromMemory(
		const std::string& filename,
		ID3D11Device* pD3DDevice,
		OUT ID3D11VertexShader** ppShader,
		D3D11_INPUT_ELEMENT_DESC pLayoutDesc[],
		UINT NumElements,
		OUT ID3D11InputLayout** g_pLayout);
	bool CreatePixelShaderFromMemory(
		const std::string& filename,
		ID3D11Device* pD3DDevice,
		OUT ID3D11PixelShader** ppShader);
	bool CreateGeometryShaderFromMemory(
		const std::string& filename,
		ID3D11Device* pD3DDevice,
		OUT ID3D11GeometryShader** ppShader);
	bool CreateHullShaderFromMemory(
		const std::string& filename,
		ID3D11Device* pD3DDevice,
		OUT ID3D11HullShader** ppShader);
	bool CreateDomainShaderFromMemory(
		const std::string& filename,
		ID3D11Device* pD3DDevice,
		OUT ID3D11DomainShader** ppShader);
	bool CreateComputeShaderFromMemory(
		const std::string& filename,
		ID3D11Device* pD3DDevice,
		OUT ID3D11ComputeShader** ppShader);
};
