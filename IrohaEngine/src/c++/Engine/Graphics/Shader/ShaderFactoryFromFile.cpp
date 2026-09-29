#include "ShaderFactoryFromFile.h"

//シェーダーを作成する
bool ShaderFactoryFromFile::CreateVertexShaderFromFile(
	const std::string& filename,
	LPCSTR szEntryPoint,
	LPCSTR szShaderModel,
	ID3D11Device* pD3DDevice,
	OUT ID3D11VertexShader** ppShader,
	D3D11_INPUT_ELEMENT_DESC pLayoutDesc[],
	UINT NumElements,
	OUT ID3D11InputLayout** g_pLayout)
{
	SetShaderDirectory();
	ShaderUtility shaderUtility;
	ID3DBlob* pBlob;
	// マルチバイト文字列からワイド文字列へ変換
	WCHAR path[256];
	size_t len = 0;
	mbstowcs_s(&len, path, 256, filename.c_str(), _TRUNCATE);
	// ファイルからコンパイル
	if (FAILED(shaderUtility.CompileShaderFromFile(path, szEntryPoint, szShaderModel, &pBlob)))
	{
		MessageBox(nullptr,
			L"HLSLファイルをコンパイルできません。HLSLファイルを含むディレクトリからこの実行可能ファイルを実行してください。", L"Error", MB_OK);
		SAFE_RELEASE(pBlob);
		return false;
	}

	// シェーダ生成
	HRESULT hr = pD3DDevice->CreateVertexShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), NULL, ppShader);
	if (FAILED(hr))
	{
		SAFE_RELEASE(pBlob);
		return false;
	}

	//入力レイアウト作成
	hr = pD3DDevice->CreateInputLayout(pLayoutDesc, NumElements, pBlob->GetBufferPointer(),
		pBlob->GetBufferSize(), g_pLayout);
	if (FAILED(hr))
		return false;

	SAFE_RELEASE(pBlob);
	return true;
}
bool ShaderFactoryFromFile::CreatePixelShaderFromFile
(
	const std::string& filename,
	LPCSTR szEntryPoint,
	LPCSTR szShaderModel,
	ID3D11Device* pD3DDevice,
	OUT ID3D11PixelShader** ppShader
)
{
	SetShaderDirectory();
	ShaderUtility shaderUtility;
	ID3DBlob* pBlob;
	// マルチバイト文字列からワイド文字列へ変換
	WCHAR path[256];
	size_t len = 0;
	mbstowcs_s(&len, path, 256, filename.c_str(), _TRUNCATE);
	// ファイルからコンパイル
	if (FAILED(shaderUtility.CompileShaderFromFile(path, szEntryPoint, szShaderModel, &pBlob)))
	{
		MessageBox(nullptr,
			L"HLSLファイルをコンパイルできません。HLSLファイルを含むディレクトリからこの実行可能ファイルを実行してください。", L"Error", MB_OK);
		SAFE_RELEASE((pBlob));
		return false;
	}

	// シェーダ生成
	HRESULT hr = pD3DDevice->CreatePixelShader((pBlob)->GetBufferPointer(), (pBlob)->GetBufferSize(), NULL, ppShader);
	if (FAILED(hr))
	{
		SAFE_RELEASE((pBlob));
		return false;
	}

	return true;
}

bool ShaderFactoryFromFile::CreateGeometryShaderFromFile
(
	const std::string& filename,
	LPCSTR szEntryPoint,
	LPCSTR szShaderModel,
	ID3D11Device* pD3DDevice,
	OUT ID3D11GeometryShader** ppShader
)
{
	SetShaderDirectory();
	ShaderUtility shaderUtility;
	ID3DBlob* pBlob;
	WCHAR path[256];
	size_t len = 0;
	mbstowcs_s(&len, path, 256, filename.c_str(), _TRUNCATE);
	// ファイルからコンパイル
	if (FAILED(shaderUtility.CompileShaderFromFile(path, szEntryPoint, szShaderModel, &pBlob)))
	{
		MessageBox(nullptr,
			L"HLSLファイルをコンパイルできません。HLSLファイルを含むディレクトリからこの実行可能ファイルを実行してください。", L"Error", MB_OK);
		SAFE_RELEASE((pBlob));
		return false;
	}

	// シェーダ生成
	HRESULT hr = pD3DDevice->CreateGeometryShader((pBlob)->GetBufferPointer(), (pBlob)->GetBufferSize(), NULL, ppShader);
	if (FAILED(hr))
	{
		SAFE_RELEASE((pBlob));
		return false;
	}

	return true;
}

bool ShaderFactoryFromFile::CreateHullShaderFromFile
(
	const std::string& filename,
	LPCSTR szEntryPoint,
	LPCSTR szShaderModel,
	ID3D11Device* pD3DDevice,
	OUT ID3D11HullShader** ppShader
)
{
	SetShaderDirectory();
	ShaderUtility shaderUtility;
	ID3DBlob* pBlob;
	WCHAR path[256];
	size_t len = 0;
	mbstowcs_s(&len, path, 256, filename.c_str(), _TRUNCATE);
	// ファイルからコンパイル
	if (FAILED(shaderUtility.CompileShaderFromFile(path, szEntryPoint, szShaderModel, &pBlob)))
	{
		MessageBox(nullptr,
			L"HLSLファイルをコンパイルできません。HLSLファイルを含むディレクトリからこの実行可能ファイルを実行してください。", L"Error", MB_OK);
		SAFE_RELEASE((pBlob));
		return false;
	}

	// シェーダ生成
	HRESULT hr = pD3DDevice->CreateHullShader((pBlob)->GetBufferPointer(), (pBlob)->GetBufferSize(), NULL, ppShader);
	if (FAILED(hr))
	{
		SAFE_RELEASE((pBlob));
		return false;
	}

	return true;
}

bool ShaderFactoryFromFile::CreateDomainShaderFromFile
(
	const std::string& filename,
	LPCSTR szEntryPoint,
	LPCSTR szShaderModel,
	ID3D11Device* pD3DDevice,
	OUT ID3D11DomainShader** ppShader
)
{
	SetShaderDirectory();
	ShaderUtility shaderUtility;
	ID3DBlob* pBlob;
	WCHAR path[256];
	size_t len = 0;
	mbstowcs_s(&len, path, 256, filename.c_str(), _TRUNCATE);
	// ファイルからコンパイル
	if (FAILED(shaderUtility.CompileShaderFromFile(path, szEntryPoint, szShaderModel, &pBlob)))
	{
		MessageBox(nullptr,
			L"HLSLファイルをコンパイルできません。HLSLファイルを含むディレクトリからこの実行可能ファイルを実行してください。", L"Error", MB_OK);
		SAFE_RELEASE((pBlob));
		return false;
	}

	// シェーダ生成
	HRESULT hr = pD3DDevice->CreateDomainShader((pBlob)->GetBufferPointer(), (pBlob)->GetBufferSize(), NULL, ppShader);
	if (FAILED(hr))
	{
		SAFE_RELEASE((pBlob));
		return false;
	}

	return true;
}

bool ShaderFactoryFromFile::CreateComputeShaderFromFile
(
	const std::string& filename,
	LPCSTR szEntryPoint,
	LPCSTR szShaderModel,
	ID3D11Device* pD3DDevice,
	OUT ID3D11ComputeShader** ppShader
)
{
	SetShaderDirectory();
	ShaderUtility shaderUtility;
	ID3DBlob* pBlob;
	WCHAR path[256];
	size_t len = 0;
	mbstowcs_s(&len, path, 256, filename.c_str(), _TRUNCATE);
	// ファイルからコンパイル
	if (FAILED(shaderUtility.CompileShaderFromFile(path, szEntryPoint, szShaderModel, &pBlob)))
	{
		MessageBox(nullptr,
			L"HLSLファイルをコンパイルできません。HLSLファイルを含むディレクトリからこの実行可能ファイルを実行してください。", L"Error", MB_OK);
		SAFE_RELEASE((pBlob));
		return false;
	}

	// シェーダ生成
	HRESULT hr = pD3DDevice->CreateComputeShader((pBlob)->GetBufferPointer(), (pBlob)->GetBufferSize(), NULL, ppShader);
	if (FAILED(hr))
	{
		SAFE_RELEASE((pBlob));
		return false;
	}

	return true;
}


