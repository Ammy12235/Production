#pragma once
#include "Core/Base.h"

#include "ShaderUtility.h"

class ShaderFactoryFromFile
{
public:
	//シェーダをコンパイルして作成する
	bool CreateVertexShaderFromFile(
		const std::string& filename,
		LPCSTR szEntryPoint,
		LPCSTR szShaderModel,
		ID3D11Device* pD3DDevice,
		OUT ID3D11VertexShader** ppShader,
		D3D11_INPUT_ELEMENT_DESC pLayoutDesc[],
		UINT NumElements,
		OUT ID3D11InputLayout** g_pLayout);
	bool CreatePixelShaderFromFile(
		const std::string& filename,
		LPCSTR szEntryPoint,
		LPCSTR szShaderModel,
		ID3D11Device* pD3DDevice,
		OUT ID3D11PixelShader** ppShader);
	bool CreateGeometryShaderFromFile(
		const std::string& filename,
		LPCSTR szEntryPoint,
		LPCSTR szShaderModel,
		ID3D11Device* pD3DDevice,
		OUT ID3D11GeometryShader** ppShader);
	bool CreateHullShaderFromFile(
		const std::string& filename,
		LPCSTR szEntryPoint,
		LPCSTR szShaderModel,
		ID3D11Device* pD3DDevice,
		OUT ID3D11HullShader** ppShader);
	bool CreateDomainShaderFromFile(
		const std::string& filename,
		LPCSTR szEntryPoint,
		LPCSTR szShaderModel,
		ID3D11Device* pD3DDevice,
		OUT ID3D11DomainShader** ppShader);
	bool CreateComputeShaderFromFile(
		const std::string& filename,
		LPCSTR szEntryPoint,
		LPCSTR szShaderModel,
		ID3D11Device* pD3DDevice,
		OUT ID3D11ComputeShader** ppShader);
};
