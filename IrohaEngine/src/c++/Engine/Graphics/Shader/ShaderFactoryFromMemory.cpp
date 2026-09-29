#include "ShaderFactoryFromMemory.h"
#include "ShaderBlob.h"

BOOL ShaderFactoryFromMemory::ReadShader(const std::string& filename, tBlob* sObj)
{

	FILE* fp;
	int ret = fopen_s(&fp, filename.c_str(), "rb");
	if (ret != 0) {
		return -1;
	}

	fseek(fp, 0, SEEK_END);
	sObj->Size = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	sObj->pData = malloc(sObj->Size);
	fread(sObj->pData, 1, sObj->Size, fp);
	fclose(fp);

	return 0;
}

bool ShaderFactoryFromMemory::CreateVertexShaderFromMemory(
	const std::string& filename,
	ID3D11Device* pD3DDevice,
	OUT ID3D11VertexShader** ppShader,
	D3D11_INPUT_ELEMENT_DESC pLayoutDesc[],
	UINT NumElements,
	OUT ID3D11InputLayout** g_pLayout)
{
	SetShaderDirectory();
	// *****************************************************************************************************************
	// 頂点シェーダーの作成
	// *****************************************************************************************************************
	tBlob cso;

	if (ReadShader(filename, &cso) != 0)
	{
		MessageBox(nullptr,
			L"プリコンパイル済みhlsl(/vs)を読み込めませんでした。", L"Error", MB_OK);
		return false;
	}

	//シェーダを生成
	HRESULT hr = pD3DDevice->CreateVertexShader(cso.pData, cso.Size, NULL, ppShader);
	if (FAILED(hr))
	{
		return false;
	}

	//入力レイアウト作成
	hr = pD3DDevice->CreateInputLayout(pLayoutDesc, NumElements, cso.pData,
		cso.Size, g_pLayout);
	if (FAILED(hr))
		return false;
	return true;
}

//シェーダーの作成
bool ShaderFactoryFromMemory::CreatePixelShaderFromMemory(
	const std::string& filename,
	ID3D11Device* pD3DDevice,
	OUT ID3D11PixelShader** ppShader)
{
	SetShaderDirectory();

	tBlob cso;

	if (ReadShader(filename, &cso) != 0)
	{
		MessageBox(nullptr,
			L"プリコンパイル済みhlsl(/ps)を読み込めませんでした。", L"Error", MB_OK);
		return false;
	}

	HRESULT hr = pD3DDevice->CreatePixelShader(cso.pData, cso.Size, NULL, ppShader);
	if (FAILED(hr))
	{
		return false;
	}
	return true;
}

bool ShaderFactoryFromMemory::CreateGeometryShaderFromMemory(
	const std::string& filename,
	ID3D11Device* pD3DDevice,
	OUT ID3D11GeometryShader** ppShader)
{
	SetShaderDirectory();

	tBlob cso;

	if (ReadShader(filename, &cso) != 0)
	{
		MessageBox(nullptr,
			L"プリコンパイル済みhlsl(/gs)を読み込めませんでした。", L"Error", MB_OK);
		return false;
	}

	HRESULT hr = pD3DDevice->CreateGeometryShader(cso.pData, cso.Size, NULL, ppShader);
	if (FAILED(hr))
	{
		return false;
	}
	return true;
}
bool ShaderFactoryFromMemory::CreateHullShaderFromMemory(
	const std::string& filename,
	ID3D11Device* pD3DDevice,
	OUT ID3D11HullShader** ppShader)
{
	SetShaderDirectory();

	tBlob cso;

	if (ReadShader(filename, &cso) != 0)
	{
		MessageBox(nullptr,
			L"プリコンパイル済みhlsl(/hs)を読み込めませんでした。", L"Error", MB_OK);
		return false;
	}

	HRESULT hr = pD3DDevice->CreateHullShader(cso.pData, cso.Size, NULL, ppShader);
	if (FAILED(hr))
	{
		return false;
	}
	return true;
}
bool ShaderFactoryFromMemory::CreateDomainShaderFromMemory(
	const std::string& filename,
	ID3D11Device* pD3DDevice,
	OUT ID3D11DomainShader** ppShader)
{
	SetShaderDirectory();

	tBlob cso;

	if (ReadShader(filename, &cso) != 0)
	{
		MessageBox(nullptr,
			L"プリコンパイル済みhlsl(/ds)を読み込めませんでした。", L"Error", MB_OK);
		return false;
	}

	HRESULT hr = pD3DDevice->CreateDomainShader(cso.pData, cso.Size, NULL, ppShader);
	if (FAILED(hr))
	{
		return false;
	}
	return true;
}
bool ShaderFactoryFromMemory::CreateComputeShaderFromMemory(
	const std::string& filename,
	ID3D11Device* pD3DDevice,
	OUT ID3D11ComputeShader** ppShader)
{
	SetShaderDirectory();

	tBlob cso;

	if (ReadShader(filename, &cso) != 0)
	{
		MessageBox(nullptr,
			L"プリコンパイル済みhlsl(/cs)を読み込めませんでした。", L"Error", MB_OK);
		return false;
	}

	HRESULT hr = pD3DDevice->CreateComputeShader(cso.pData, cso.Size, NULL, ppShader);
	if (FAILED(hr))
	{
		return false;
	}
	return true;
}
