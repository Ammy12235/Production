#include "BaseShading.h"
#include "DEFINE.h"
#include "Base.h"

BASE_SHADING::BASE_SHADING()
{

}

BASE_SHADING::~BASE_SHADING()
{
	CleanupDevice();
}

void BASE_SHADING::CleanupDevice()
{
	SAFE_RELEASE(m_pLayout);
	SAFE_RELEASE(m_pConstantBuffer);
	SAFE_RELEASE(m_pVertexBuffer);
	SAFE_RELEASE(m_pVertexShader);
	SAFE_RELEASE(m_pPixelShader2);
	SAFE_RELEASE(m_pPixelShader);
}

//シェーダファイルをコンパイルする
HRESULT BASE_SHADING::CompileShaderFromFile(const WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut)
{
	HRESULT hr = S_OK;

	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
	dwShaderFlags |= D3DCOMPILE_DEBUG;

	dwShaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	ID3DBlob* pErrorBlob = nullptr;
	hr = D3DCompileFromFile(szFileName, nullptr, nullptr, szEntryPoint, szShaderModel,
		dwShaderFlags, 0, ppBlobOut, &pErrorBlob);
	if (FAILED(hr))
	{
		if (pErrorBlob)
		{
			OutputDebugStringA(reinterpret_cast<const char*>(pErrorBlob->GetBufferPointer()));
			pErrorBlob->Release();
		}
		return hr;
	}
	if (pErrorBlob) pErrorBlob->Release();

	return S_OK;
}
// 初期化
HRESULT BASE_SHADING::InitPreCom(ID3D11Device* pD3DDevice, ID3D11DeviceContext* pD3DDeviceContext
	, const BYTE* pVS, size_t VSSize
	, const BYTE* pPS, size_t PSSize
)
{
	HRESULT hr = E_FAIL;

	if (pD3DDevice == NULL) return hr;

	// *****************************************************************************************************************
	// 頂点シェーダーの作成
	// *****************************************************************************************************************

	hr = pD3DDevice->CreateVertexShader(pVS, VSSize, NULL, &m_pVertexShader);
	if (FAILED(hr))return hr;

	// インプットレイアウトの定義
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXUV",   0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },

	};

	hr = pD3DDevice->CreateInputLayout(layout, _countof(layout), pVS, VSSize, &m_pLayout);
	if (FAILED(hr)) return hr;
	// インプットレイアウトの設定
	pD3DDeviceContext->IASetInputLayout(m_pLayout);
	// *****************************************************************************************************************
	// ピクセルェーダーの作成
	// *****************************************************************************************************************

	hr = pD3DDevice->CreatePixelShader(pPS, PSSize, nullptr, &m_pPixelShader);
	if (FAILED(hr)) return hr;


	// *****************************************************************************************************************
	// 頂点バッファを作成
	// *****************************************************************************************************************

	D3D11_BUFFER_DESC vbDesc = {};
	UINT stride = sizeof(Vertex2D);
	UINT offset = 0;
	vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;	// デバイスにバインドするときの種類(頂点バッファ、インデックスバッファ、定数バッファなど)
	vbDesc.ByteWidth = sizeof(Vertex2D) * 6;	// 作成するバッファのバイトサイズ
	vbDesc.MiscFlags = 0;							// その他のフラグ
	vbDesc.StructureByteStride = 0;					// 構造化バッファの場合、その構造体のサイズ

	vbDesc.Usage = D3D11_USAGE_DYNAMIC;				// 作成するバッファの使用法
	vbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	hr = pD3DDevice->CreateBuffer(&vbDesc, nullptr, &m_pVertexBuffer);

	// 頂点バッファを描画で使えるようにセットする
	pD3DDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

	// プロミティブ・トポロジーをセット
	pD3DDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	// 
	// *****************************************************************************************************************
	// 定数バッファを作成
	// *****************************************************************************************************************

	D3D11_BUFFER_DESC BufferDesc;

	::ZeroMemory(&BufferDesc, sizeof(BufferDesc));
	BufferDesc.ByteWidth = sizeof(CBUFFER0);        // バッファサイズ
	BufferDesc.Usage = D3D11_USAGE_DYNAMIC;       // リソース使用法を特定する
	BufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;// バッファの種類
	BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;    // CPU アクセス
	BufferDesc.MiscFlags = 0;                         // その他のフラグも設定しない
	hr = pD3DDevice->CreateBuffer(&BufferDesc, NULL, &m_pConstantBuffer);
	if (FAILED(hr)) return hr;

	g_World = XMMatrixIdentity();//ワールド座標系初期化
	hr = S_OK;
	return hr;
}

HRESULT BASE_SHADING::Init(ID3D11Device* pD3DDevice, ID3D11DeviceContext* pD3DDeviceContext)
{
	HRESULT hr = E_FAIL;

	ID3D10Blob* pVSBlob = NULL, * pPSBlob = NULL;
	Log("	シェーダー初期化開始\n");
	// 行列を列優先で設定し、古い形式の記述を許可しないようにする
	UINT Flag1 = D3D10_SHADER_PACK_MATRIX_COLUMN_MAJOR | D3D10_SHADER_ENABLE_STRICTNESS;
	// 最適化レベルを設定する
#if defined(DEBUG) || defined(_DEBUG)
	Flag1 |= D3D10_SHADER_OPTIMIZATION_LEVEL0;
#else
	Flag1 |= D3D10_SHADER_OPTIMIZATION_LEVEL3;
#endif
	SetShaderDirectory();

	// バーテックスシェーダのコンパイル
	hr = CompileShaderFromFile(hlslSrc, vs_main, "vs_5_0", &pVSBlob);
	if (FAILED(hr))
	{
		MessageBox(nullptr,
			L"HLSLファイルをコンパイルできません。HLSLファイルを含むディレクトリからこの実行可能ファイルを実行してください。", L"Error", MB_OK);
		return hr;
	}

	// バーテックスシェーダの作成
	hr = pD3DDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &m_pVertexShader);
	if (FAILED(hr))
	{

		pVSBlob->Release();
		return hr;

	}
	// インプットレイアウトの定義
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXUV",   0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },

	};
	UINT numElements = ARRAYSIZE(layout);

	// インプットレイアウトの作成
	hr = pD3DDevice->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(),
		pVSBlob->GetBufferSize(), &m_pLayout);
	if (FAILED(hr))
		return hr;

	// インプットレイアウトの設定
	pD3DDeviceContext->IASetInputLayout(m_pLayout);

	// ピクセルシェーダのコンパイル
	hr = CompileShaderFromFile(hlslSrc, ps_main_01, "ps_5_0", &pPSBlob);
	if (FAILED(hr))
	{
		MessageBox(nullptr,
			L"HLSLファイルをコンパイルできません。HLSLファイルを含むディレクトリからこの実行可能ファイルを実行してください。", L"Error", MB_OK);
		return hr;
	}

	// ピクセルシェーダの作成(単色描画)
	hr = pD3DDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &m_pPixelShader);
	if (FAILED(hr))
	{

		return hr;
	}
	pPSBlob = nullptr;

	// ピクセルシェーダ2のコンパイル
	hr = CompileShaderFromFile(hlslSrc, ps_main_02, "ps_5_0", &pPSBlob);
	if (FAILED(hr))
	{
		MessageBox(nullptr,
			L"HLSLファイルをコンパイルできません。HLSLファイルを含むディレクトリからこの実行可能ファイルを実行してください。", L"Error", MB_OK);
		return hr;
	}

	// ピクセルシェーダの作成(テクスチャ付き)
	hr = pD3DDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &m_pPixelShader2);
	if (FAILED(hr))
	{

		return hr;
	}
	// *****************************************************************************************************************
	// 頂点バッファを作成
	// *****************************************************************************************************************

	D3D11_BUFFER_DESC vbDesc = {};
	UINT stride = sizeof(Vertex2D);
	UINT offset = 0;
	vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;	// デバイスにバインドするときの種類(頂点バッファ、インデックスバッファ、定数バッファなど)
	vbDesc.ByteWidth = sizeof(Vertex2D) * 6;	// 作成するバッファのバイトサイズ
	vbDesc.MiscFlags = 0;							// その他のフラグ
	vbDesc.StructureByteStride = 0;					// 構造化バッファの場合、その構造体のサイズ

	vbDesc.Usage = D3D11_USAGE_DYNAMIC;				// 作成するバッファの使用法
	vbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	hr = pD3DDevice->CreateBuffer(&vbDesc, nullptr, &m_pVertexBuffer);

	// 頂点バッファを描画で使えるようにセットする
	pD3DDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

	// プロミティブ・トポロジーをセット
	pD3DDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	// 
	// *****************************************************************************************************************
	// 定数バッファを作成
	// *****************************************************************************************************************

	D3D11_BUFFER_DESC BufferDesc;
	//頂点用
	::ZeroMemory(&BufferDesc, sizeof(BufferDesc));
	BufferDesc.ByteWidth = sizeof(CBUFFER0);        // バッファサイズ
	BufferDesc.Usage = D3D11_USAGE_DYNAMIC;       // リソース使用法を特定する
	BufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;// バッファの種類
	BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;    // CPU アクセス
	BufferDesc.MiscFlags = 0;                         // その他のフラグも設定しない
	hr = pD3DDevice->CreateBuffer(&BufferDesc, NULL, &m_pConstantBuffer);
	if (FAILED(hr)) return hr;

	g_World = XMMatrixIdentity();//ワールド座標系初期化

	//２D描画モード
	hr=ChangeMode_2D( pD3DDevice, pD3DDeviceContext);
	if (FAILED(hr))goto EXIT;

	Log("	シェーダー初期化完了\n");
	hr = S_OK;
EXIT:
	SAFE_RELEASE(pVSBlob);
	SAFE_RELEASE(pPSBlob);

	return hr;
}

//=====================================
// 描画モードを2Dにする
//=====================================
HRESULT BASE_SHADING::ChangeMode_2D(ID3D11Device* pD3DDevice,ID3D11DeviceContext* pD3DDeviceContext)
{
	HRESULT hr=E_FAIL;
	// 頂点シェーダーをデバイスに設定する。
	pD3DDeviceContext->VSSetShader(m_pVertexShader, nullptr, 0);
	// ピクセルシェーダーをデバイスに設定する。
	pD3DDeviceContext->PSSetShader(m_pPixelShader, nullptr, 0);

	// プロミティブ・トポロジーをセット
	pD3DDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// サンプラーステートを作成しセットする
	{
		// 異方性フィルタリング補間、Wrapモード
		D3D11_SAMPLER_DESC desc = {};
		desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;	// 何もフィルタリングしない
		desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;		// テクスチャアドレッシングモードをWrapに
		desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;		// テクスチャアドレッシングモードをWrapに
		desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;		// テクスチャアドレッシングモードをWrapに
		desc.MipLODBias = 0;
		desc.MaxAnisotropy = 0;
		desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		desc.BorderColor[0] = desc.BorderColor[1] = desc.BorderColor[2] = desc.BorderColor[3] = 0;
		desc.MinLOD = 0;
		desc.MaxLOD = D3D11_FLOAT32_MAX;

		// ステートオブジェクト作成
		ComPtr<ID3D11SamplerState> state;
		hr=pD3DDevice->CreateSamplerState(&desc, &state);
		if (FAILED(hr))return hr;
		// 各シェーダーの0番目にセット(実際は必要なシェーダーだけセットしてください)
		pD3DDeviceContext->VSSetSamplers(0, 1, state.GetAddressOf()); // 頂点シェーダーの0番目にセット
		pD3DDeviceContext->PSSetSamplers(0, 1, state.GetAddressOf()); // ピクセルシェーダーの0番目にセット
		
	}

	hr = S_OK;
	return hr;
}

void BASE_SHADING::WriteVertexInfo2D(ID3D11DeviceContext* pD3DDeviceContext,Vertex2D* v, int arraySize)
{
	// インプットレイアウトの設定
	pD3DDeviceContext->IASetInputLayout(m_pLayout);

	UINT stride = sizeof(Vertex2D);
	UINT offset = 0;
	// 頂点バッファを描画で使えるようにセットする
	pD3DDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

	// 頂点バッファにデータを書き込む
	D3D11_MAPPED_SUBRESOURCE pData;
	if (SUCCEEDED(pD3DDeviceContext->Map(m_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &pData)))
	{
		// データコピー
		memcpy_s(pData.pData, arraySize, &v[0], arraySize);

		pD3DDeviceContext->Unmap(m_pVertexBuffer, 0);
	}

	//定数バッファにデータを書き込む
	CBUFFER0 cb;
	if (SUCCEEDED(pD3DDeviceContext->Map(m_pConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &pData)))
	{
		cb.matWVP = XMMatrixTranspose(g_World);
		cb.viewPortWidth = Define::WIN_W;
		cb.viewPortHeight = Define::WIN_H;
		cb.alpha = alpha;
		memcpy_s(pData.pData, pData.RowPitch, (void*)(&cb), sizeof(cb));
		pD3DDeviceContext->Unmap(m_pConstantBuffer, 0);
	}

	pD3DDeviceContext->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);
	pD3DDeviceContext->PSSetConstantBuffers(0, 1, &m_pConstantBuffer);

}


void BASE_SHADING::SetAlpha(ID3D11DeviceContext* pD3DDeviceContext,int value)
{
	if (value < 0)value = 0; if (value > 255)value = 255;
	float Alpha = (float)value / 255;alpha = Alpha;
}


