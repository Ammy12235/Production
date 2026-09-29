#include "ShaderFactoryFromResource.h"
#include "ShaderBlob.h"

//シェーダをリソースから作成する
bool ShaderFactoryFromResource::CreateVertexShaderFromResource(
	const std::string& filename,
	ID3D11Device* pD3DDevice,
	OUT ID3D11VertexShader** ppShader,
	D3D11_INPUT_ELEMENT_DESC pLayoutDesc[],
	UINT NumElements,
	OUT ID3D11InputLayout** g_pLayout)
{
	SetShaderDirectory();
	HRESULT hr = E_FAIL;

	HANDLE handle;
	HRSRC hRes;
	tBlob cso;

	//コンパイル済みシェーダーファイルをリソースから読み込む
	hRes = FindResource(NULL, MAKEINTRESOURCE(atoi(filename.c_str())), L"SHADER");
	if (hRes == NULL)
	{
		return false;
	}
	handle = LoadResource(NULL, hRes);
	if (!handle) {
		return false;
	}
	cso.pData = (LPBYTE)LockResource(handle);
	cso.Size = SizeofResource(NULL, hRes);

	hr = pD3DDevice->CreateVertexShader(cso.pData, cso.Size, NULL, ppShader);
	if (FAILED(hr))return false;

	//入力レイアウト作成
	hr = pD3DDevice->CreateInputLayout(pLayoutDesc, NumElements, cso.pData,
		cso.Size, g_pLayout);
	if (FAILED(hr))
		return false;
	
	return true;
}

bool ShaderFactoryFromResource::CreatePixelShaderFromResource(
	const std::string& filename,
	ID3D11Device* pD3DDevice,
	OUT ID3D11PixelShader** ppShader)
{
	SetShaderDirectory();
	HRESULT hr = E_FAIL;

	HANDLE handle;
	HRSRC hRes;
	tBlob cso;

	//コンパイル済みシェーダーファイルをリソースから読み込む
	hRes = FindResource(NULL, MAKEINTRESOURCE(atoi(filename.c_str())), L"SHADER");
	if (hRes == NULL)
	{
		return false;
	}
	handle = LoadResource(NULL, hRes);
	if (!handle) {
		return false;
	}
	cso.pData = (LPBYTE)LockResource(handle);
	cso.Size = SizeofResource(NULL, hRes);

	hr = pD3DDevice->CreatePixelShader(cso.pData, cso.Size, NULL, ppShader);
	if (FAILED(hr))return false;
	return true;
}
bool ShaderFactoryFromResource::CreateGeometryShaderFromResource(
	const std::string& filename,
	ID3D11Device* pD3DDevice,
	OUT ID3D11GeometryShader** ppShader)
{
	SetShaderDirectory();
	HRESULT hr = E_FAIL;

	HANDLE handle;
	HRSRC hRes;
	tBlob cso;

	//コンパイル済みシェーダーファイルをリソースから読み込む
	hRes = FindResource(NULL, MAKEINTRESOURCE(atoi(filename.c_str())), L"SHADER");
	if (hRes == NULL)
	{
		return false;
	}
	handle = LoadResource(NULL, hRes);
	if (!handle) {
		return false;
	}
	cso.pData = (LPBYTE)LockResource(handle);
	cso.Size = SizeofResource(NULL, hRes);

	hr = pD3DDevice->CreateGeometryShader(cso.pData, cso.Size, NULL, ppShader);
	if (FAILED(hr))return false;

	return true;
}
bool ShaderFactoryFromResource::CreateHullShaderFromResource(
	const std::string& filename,
	ID3D11Device* pD3DDevice,
	OUT ID3D11HullShader** ppShader)
{
	SetShaderDirectory();
	HRESULT hr = E_FAIL;

	HANDLE handle;
	HRSRC hRes;
	tBlob cso;

	//コンパイル済みシェーダーファイルをリソースから読み込む
	hRes = FindResource(NULL, MAKEINTRESOURCE(atoi(filename.c_str())), L"SHADER");
	if (hRes == NULL)
	{
		return false;
	}
	handle = LoadResource(NULL, hRes);
	if (!handle) {
		return false;
	}
	cso.pData = (LPBYTE)LockResource(handle);
	cso.Size = SizeofResource(NULL, hRes);

	hr = pD3DDevice->CreateHullShader(cso.pData, cso.Size, NULL, ppShader);
	if (FAILED(hr))return false;

	return true;
}
bool ShaderFactoryFromResource::CreateDomainShaderFromResource(
	const std::string& filename,
	ID3D11Device* pD3DDevice,
	OUT ID3D11DomainShader** ppShader)
{
	SetShaderDirectory();
	HRESULT hr = E_FAIL;

	HANDLE handle;
	HRSRC hRes;
	tBlob cso;

	//コンパイル済みシェーダーファイルをリソースから読み込む
	hRes = FindResource(NULL, MAKEINTRESOURCE(atoi(filename.c_str())), L"SHADER");
	if (hRes == NULL)
	{
		return false;
	}
	handle = LoadResource(NULL, hRes);
	if (!handle) {
		return false;
	}
	cso.pData = (LPBYTE)LockResource(handle);
	cso.Size = SizeofResource(NULL, hRes);

	hr = pD3DDevice->CreateDomainShader(cso.pData, cso.Size, NULL, ppShader);
	if (FAILED(hr))return false;

	return true;
}

bool ShaderFactoryFromResource::CreateComputeShaderFromResource(
	const std::string& filename,
	ID3D11Device* pD3DDevice,
	OUT ID3D11ComputeShader** ppShader)
{
	SetShaderDirectory();
	HRESULT hr = E_FAIL;

	HANDLE handle;
	HRSRC hRes;
	tBlob cso;

	//コンパイル済みシェーダーファイルをリソースから読み込む
	hRes = FindResource(NULL, MAKEINTRESOURCE(atoi(filename.c_str())), L"SHADER");
	if (hRes == NULL)
	{
		return false;
	}
	handle = LoadResource(NULL, hRes);
	if (!handle) {
		return false;
	}
	cso.pData = (LPBYTE)LockResource(handle);
	cso.Size = SizeofResource(NULL, hRes);

	hr = pD3DDevice->CreateComputeShader(cso.pData, cso.Size, NULL, ppShader);
	if (FAILED(hr))return false;

	return true;
}
