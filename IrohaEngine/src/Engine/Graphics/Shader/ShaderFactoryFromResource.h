#pragma once
#include "Core/Base.h"

#include "ShaderUtility.h"

class ShaderFactoryFromResource
{
public:
	//シェーダをリソースから作成する
	bool CreateVertexShaderFromResource(
		const std::string& filename,
		ID3D11Device* pD3DDevice,
		OUT ID3D11VertexShader** ppShader,
		D3D11_INPUT_ELEMENT_DESC pLayoutDesc[],
		UINT NumElements,
		OUT ID3D11InputLayout** g_pLayout);
	bool CreatePixelShaderFromResource(
		const std::string& filename,
		ID3D11Device* pD3DDevice,
		OUT ID3D11PixelShader** ppShader);
	bool CreateGeometryShaderFromResource(
		const std::string& filename,
		ID3D11Device* pD3DDevice,
		OUT ID3D11GeometryShader** ppShader);
	bool CreateHullShaderFromResource(
		const std::string& filename,
		ID3D11Device* pD3DDevice,
		OUT ID3D11HullShader** ppShader);
	bool CreateDomainShaderFromResource(
		const std::string& filename,
		ID3D11Device* pD3DDevice,
		OUT ID3D11DomainShader** ppShader);
	bool CreateComputeShaderFromResource(
		const std::string& filename,
		ID3D11Device* pD3DDevice,
		OUT ID3D11ComputeShader** ppShader);
};
