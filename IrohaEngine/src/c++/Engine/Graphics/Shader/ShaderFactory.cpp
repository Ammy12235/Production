#include "ShaderFactory.h"
#include "ShaderFactoryFromFile.h"
#include "ShaderFactoryFromMemory.h"
#include "ShaderFactoryFromResource.h"

bool ShaderFactory::init(ID3D11Device* pD3DDevice, ID3D11DeviceContext* pD3DDeviceContext)
{
	_pD3DDevice = pD3DDevice;
	_pD3DDeviceContext = pD3DDeviceContext;

	return true;
}

HRESULT ShaderFactory::CreateVertexShader(eGenerateType generateType,
	ShaderDesc shaderDesc,
	OUT ID3D11VertexShader** ppShader,
	D3D11_INPUT_ELEMENT_DESC pLayoutDesc[],
	UINT NumElements,
	OUT ID3D11InputLayout** g_pLayout)
{
	HRESULT hr = E_FAIL;

	//シェーダーをいずれかの方法で作成
	switch (generateType)
	{
	case FROM_FILE:
		ShaderFactoryFromFile shaderFactoryFromFile;
		if (!shaderFactoryFromFile.CreateVertexShaderFromFile(
			shaderDesc.fileName,
			shaderDesc.entryPointName,
			shaderDesc.shaderModel,
			_pD3DDevice,
			ppShader,
			pLayoutDesc,
			NumElements,
			g_pLayout))
			return hr;
		else break;
	case FROM_MEMORY:
		ShaderFactoryFromMemory shaderFactoryFromMemory;
		if (!shaderFactoryFromMemory.CreateVertexShaderFromMemory(
			shaderDesc.fileName,
			_pD3DDevice,
			ppShader,
			pLayoutDesc,
			NumElements,
			g_pLayout))
			return hr;
		else break;
	case FROM_RESOURCE:
		ShaderFactoryFromResource shaderFactoryFromResource;
		if (!shaderFactoryFromResource.CreateVertexShaderFromResource(
			shaderDesc.fileName,
			_pD3DDevice,
			ppShader,
			pLayoutDesc,
			NumElements,
			g_pLayout))
			return hr;
		else break;
	default:
		hr = E_FAIL;
		return hr;
	}
	return S_OK;
}

HRESULT ShaderFactory::CreatePixelShader(
	eGenerateType generateType,
	ShaderDesc shaderDesc,
	OUT ID3D11PixelShader** ppShader)
{
	HRESULT hr = E_FAIL;

	//シェーダーをいずれかの方法で作成
	switch (generateType)
	{
	case FROM_FILE:
		ShaderFactoryFromFile shaderFactoryFromFile;
		if (!shaderFactoryFromFile.CreatePixelShaderFromFile(
			shaderDesc.fileName,
			shaderDesc.entryPointName,
			shaderDesc.shaderModel,
			_pD3DDevice,
			ppShader))
			return hr;
		else break;
	case FROM_MEMORY:
		ShaderFactoryFromMemory shaderFactoryFromMemory;
		if (!shaderFactoryFromMemory.CreatePixelShaderFromMemory(
			shaderDesc.fileName,
			_pD3DDevice,
			ppShader))
			return hr;
		else break;
	case FROM_RESOURCE:
		ShaderFactoryFromResource shaderFactoryFromResource;
		if (!shaderFactoryFromResource.CreatePixelShaderFromResource(
			shaderDesc.fileName,
			_pD3DDevice,
			ppShader))
			return hr;
		else break;
	default:
		hr = E_FAIL;
		return hr;
	}
	return S_OK;
}

HRESULT ShaderFactory::CreateGeometryShader(
	eGenerateType generateType,
	ShaderDesc shaderDesc,
	OUT ID3D11GeometryShader** ppShader)
{
	HRESULT hr = E_FAIL;

	//シェーダーをいずれかの方法で作成
	switch (generateType)
	{
	case FROM_FILE:
		ShaderFactoryFromFile shaderFactoryFromFile;
		if (!shaderFactoryFromFile.CreateGeometryShaderFromFile(
			shaderDesc.fileName,
			shaderDesc.entryPointName,
			shaderDesc.shaderModel,
			_pD3DDevice,
			ppShader))
			return hr;
		else break;
	case FROM_MEMORY:
		ShaderFactoryFromMemory shaderFactoryFromMemory;
		if (!shaderFactoryFromMemory.CreateGeometryShaderFromMemory(
			shaderDesc.fileName,
			_pD3DDevice,
			ppShader))
			return hr;
		else break;
	case FROM_RESOURCE:
		ShaderFactoryFromResource shaderFactoryFromResource;
		if (!shaderFactoryFromResource.CreateGeometryShaderFromResource(
			shaderDesc.fileName,
			_pD3DDevice, 
			ppShader))
			return hr;
		else break;
	default:
		hr = E_FAIL;
		return hr;
	}
	return S_OK;
}
HRESULT ShaderFactory::CreateHullShader(
	eGenerateType generateType,
	ShaderDesc shaderDesc,
	OUT ID3D11HullShader** ppShader)
{
	HRESULT hr = E_FAIL;

	//シェーダーをいずれかの方法で作成
	switch (generateType)
	{
	case FROM_FILE:
		ShaderFactoryFromFile shaderFactoryFromFile;
		if (!shaderFactoryFromFile.CreateHullShaderFromFile(
			shaderDesc.fileName,
			shaderDesc.entryPointName,
			shaderDesc.shaderModel,
			_pD3DDevice,
			ppShader))
			return hr;
		else break;
	case FROM_MEMORY:
		ShaderFactoryFromMemory shaderFactoryFromMemory;
		if (!shaderFactoryFromMemory.CreateHullShaderFromMemory(
			shaderDesc.fileName,
			_pD3DDevice,
			ppShader))
			return hr;
		else break;
	case FROM_RESOURCE:
		ShaderFactoryFromResource shaderFactoryFromResource;
		if (!shaderFactoryFromResource.CreateHullShaderFromResource(
			shaderDesc.fileName,
			_pD3DDevice, 
			ppShader))
			return hr;
		else break;
	default:
		hr = E_FAIL;
		return hr;
	}
	return S_OK;
}
HRESULT ShaderFactory::CreateDomainShader(
	eGenerateType generateType,
	ShaderDesc shaderDesc,
	OUT ID3D11DomainShader** ppShader)
{
	HRESULT hr = E_FAIL;

	//シェーダーをいずれかの方法で作成
	switch (generateType)
	{
	case FROM_FILE:
		ShaderFactoryFromFile shaderFactoryFromFile;
		if (!shaderFactoryFromFile.CreateDomainShaderFromFile(
			shaderDesc.fileName,
			shaderDesc.entryPointName,
			shaderDesc.shaderModel,
			_pD3DDevice,
			ppShader))
			return hr;
		else break;
	case FROM_MEMORY:
		ShaderFactoryFromMemory shaderFactoryFromMemory;
		if (!shaderFactoryFromMemory.CreateDomainShaderFromMemory(
			shaderDesc.fileName,
			_pD3DDevice,
			ppShader))
			return hr;
		else break;
	case FROM_RESOURCE:
		ShaderFactoryFromResource shaderFactoryFromResource;
		if (!shaderFactoryFromResource.CreateDomainShaderFromResource(
			shaderDesc.fileName,
			_pD3DDevice,
			ppShader))
			return hr;
		else break;
	default:
		hr = E_FAIL;
		return hr;
	}
	return S_OK;
}
HRESULT ShaderFactory::CreateComputeShader(
	eGenerateType generateType,
	ShaderDesc shaderDesc,
	OUT ID3D11ComputeShader** ppShader)
{
	HRESULT hr = E_FAIL;

	//シェーダーをいずれかの方法で作成
	switch (generateType)
	{
	case FROM_FILE:
		ShaderFactoryFromFile shaderFactoryFromFile;
		if (!shaderFactoryFromFile.CreateComputeShaderFromFile(
			shaderDesc.fileName,
			shaderDesc.entryPointName,
			shaderDesc.shaderModel,
			_pD3DDevice, 
			ppShader))
			return hr;
		else break;
	case FROM_MEMORY:
		ShaderFactoryFromMemory shaderFactoryFromMemory;
		if (!shaderFactoryFromMemory.CreateComputeShaderFromMemory(
			shaderDesc.fileName,
			_pD3DDevice,
			ppShader))
			return hr;
		else break;
	case FROM_RESOURCE:
		ShaderFactoryFromResource shaderFactoryFromResource;
		if (!shaderFactoryFromResource.CreateComputeShaderFromResource(
			shaderDesc.fileName, 
			_pD3DDevice,
			ppShader))
			return hr;
		else break;
	default:
		hr = E_FAIL;
		return hr;
	}

	return S_OK;
}