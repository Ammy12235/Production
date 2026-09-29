#include "GaussianFilter.h"
#include "DEFINE.h"
#include "Base.h"

GAUSSIANFILTER::GAUSSIANFILTER()
{
	m_pConstantBuffers = NULL;
	m_pSamplerState = NULL;
	m_pRTV = NULL;
	m_Width = 0;
	m_Height = 0;
}

GAUSSIANFILTER::~GAUSSIANFILTER()
{
	RemoveDevice();
}

void GAUSSIANFILTER::RemoveDevice()
{
	SAFE_RELEASE(m_pVertexBuffer);
	SAFE_RELEASE(m_pRTV);
	SAFE_RELEASE(m_pSamplerState);
	SAFE_RELEASE(m_pConstantBuffers);
	for (int i = 0; i < 2; i++)
		SAFE_RELEASE(m_pPixelShader[i]);
	SAFE_RELEASE(m_pLayout);
	for (int i = 0; i < 2; i++)
		SAFE_RELEASE(m_pVertexShader[i]);
}

//シェーダファイルをコンパイルする
HRESULT GAUSSIANFILTER::CompileShaderFromFile(const WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut)
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
HRESULT GAUSSIANFILTER::Init(ID3D11Device* pD3DDevice, ID3D11DeviceContext* pD3DDeviceContext, TCHAR pSrcFile[], UINT Width, UINT Height)
{
	HRESULT hr = E_FAIL;

	ID3D11Texture2D* pTex2D = NULL;
	ID3D10Blob* pVSBlob = NULL, * pPSBlob = NULL;

	m_Width = Width;
	m_Height = Height;

	SetShaderDirectory();

	// バーテックスシェーダ1のコンパイル
	hr = CompileShaderFromFile(pSrcFile, vs_main_01, "vs_5_0", &pVSBlob);
	if (FAILED(hr))
	{
		MessageBox(nullptr,
			L"HLSLファイルをコンパイルできません。HLSLファイルを含むディレクトリからこの実行可能ファイルを実行してください。", L"Error", MB_OK);
		return hr;
	}

	// バーテックスシェーダ1の作成
	hr = pD3DDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &m_pVertexShader[0]);
	if (FAILED(hr))
	{

		pVSBlob->Release();
		return hr;

	}
	pVSBlob = nullptr;
	// バーテックスシェーダ2のコンパイル
	hr = CompileShaderFromFile(pSrcFile, vs_main_02, "vs_5_0", &pVSBlob);
	if (FAILED(hr))
	{
		MessageBox(nullptr,
			L"HLSLファイルをコンパイルできません。HLSLファイルを含むディレクトリからこの実行可能ファイルを実行してください。", L"Error", MB_OK);
		return hr;
	}

	// バーテックスシェーダ2の作成
	hr = pD3DDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &m_pVertexShader[1]);
	if (FAILED(hr))
	{

		pVSBlob->Release();
		return hr;

	}

	// インプットレイアウトの定義
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",   0, DXGI_FORMAT_R32G32_FLOAT, 0,  D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },

	};
	UINT numElements = ARRAYSIZE(layout);

	// インプットレイアウトの作成
	hr = pD3DDevice->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(),
		pVSBlob->GetBufferSize(), &m_pLayout);
	if (FAILED(hr))
		return hr;
	pVSBlob->Release();

	// インプットレイアウトの設定
	pD3DDeviceContext->IASetInputLayout(m_pLayout);

	// ピクセルシェーダ1のコンパイル
	hr = CompileShaderFromFile(pSrcFile, ps_main_01, "ps_5_0", &pPSBlob);
	if (FAILED(hr))
	{
		MessageBox(nullptr,
			L"HLSLファイルをコンパイルできません。HLSLファイルを含むディレクトリからこの実行可能ファイルを実行してください。", L"Error", MB_OK);
		return hr;
	}

	// ピクセルシェーダ1の作成
	hr = pD3DDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &m_pPixelShader[0]);
	if (FAILED(hr))
	{

		return hr;
	}
	pPSBlob = nullptr;

	// ピクセルシェーダ2のコンパイル
	hr = CompileShaderFromFile(pSrcFile, ps_main_02, "ps_5_0", &pPSBlob);
	if (FAILED(hr))
	{
		MessageBox(nullptr,
			L"HLSLファイルをコンパイルできません。HLSLファイルを含むディレクトリからこの実行可能ファイルを実行してください。", L"Error", MB_OK);
		return hr;
	}

	// ピクセルシェーダ2の作成
	hr = pD3DDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &m_pPixelShader[1]);
	if (FAILED(hr))
	{

		return hr;
	}
	pPSBlob->Release();

	// 定数バッファを作成
	D3D11_BUFFER_DESC BufferDesc;
	::ZeroMemory(&BufferDesc, sizeof(BufferDesc));
	BufferDesc.ByteWidth = sizeof(CBUFFER);         // バッファサイズ
	BufferDesc.Usage = D3D11_USAGE_DYNAMIC;       // リソース使用法を特定する
	BufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;// バッファの種類
	BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;    // CPU アクセス
	BufferDesc.MiscFlags = 0;                         // その他のフラグも設定しない
	// バッファを作成する
	hr = pD3DDevice->CreateBuffer(&BufferDesc, NULL, &m_pConstantBuffers);
	if (FAILED(hr))return hr;

	// サンプラーステートの設定
	D3D11_SAMPLER_DESC samplerDesc;
	
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.MipLODBias = 0;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = samplerDesc.BorderColor[1] = samplerDesc.BorderColor[2] = samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	hr = pD3DDevice->CreateSamplerState(&samplerDesc, &m_pSamplerState);
	if (FAILED(hr)) return hr;

	// レンダーターゲットビューを作成
	{
		D3D11_TEXTURE2D_DESC Tex2DDesc;
		D3D11_RENDER_TARGET_VIEW_DESC RTVDesc;

		::ZeroMemory(&Tex2DDesc, sizeof(D3D11_TEXTURE2D_DESC));
		Tex2DDesc.ArraySize = 1;
		Tex2DDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		Tex2DDesc.Usage = D3D11_USAGE_DEFAULT;
		Tex2DDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		Tex2DDesc.Width = m_Width;
		Tex2DDesc.Height = m_Height;
		Tex2DDesc.MipLevels = 1;
		Tex2DDesc.SampleDesc.Count = 1;

		::ZeroMemory(&RTVDesc, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));
		RTVDesc.Format = Tex2DDesc.Format;
		RTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		RTVDesc.Texture2D.MipSlice = 0;

		hr = pD3DDevice->CreateTexture2D(&Tex2DDesc, NULL, &pTex2D);
		if (FAILED(hr))return hr;

		hr = pD3DDevice->CreateRenderTargetView(pTex2D, &RTVDesc, &m_pRTV);
		if (FAILED(hr))return hr;

		SAFE_RELEASE(pTex2D);
	}

	// 射影座標系上での頂点座標を設定する
	VERTEX v[] = {
		 XMFLOAT3(-1,  -1, 0), XMFLOAT2(0, 1),
		  XMFLOAT3(-1,  1, 0),XMFLOAT2(0, 0),
		  XMFLOAT3(1, -1, 0), XMFLOAT2(1, 1),
		  XMFLOAT3(1, 1, 0),XMFLOAT2(1, 0)
	};

	// 頂点バッファー リソース
	::ZeroMemory(&BufferDesc, sizeof(BufferDesc));
	BufferDesc.ByteWidth = sizeof(v);               // バッファサイズ
	BufferDesc.Usage = D3D11_USAGE_DYNAMIC;       // リソース使用法を特定する
	BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;  // バッファの種類
	BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;    // CPU アクセス
	BufferDesc.MiscFlags = 0;                         // その他のフラグも設定しない
	// サブリソース( 初期値 )
	D3D11_SUBRESOURCE_DATA resource;
	resource.pSysMem = (void*)v;
	resource.SysMemPitch = 0;
	resource.SysMemSlicePitch = 0;
	// バッファを作成する
	hr = pD3DDevice->CreateBuffer(&BufferDesc, &resource, &m_pVertexBuffer);
	if (FAILED(hr))return hr;
	hr = S_OK;

EXIT:
	SAFE_RELEASE(pTex2D);

	return hr;
}

// 定数バッファを設定する
HRESULT GAUSSIANFILTER::SetConstantBuffers(ID3D11DeviceContext* pD3DDeviceContext,
	float Dispersion)
{

	HRESULT hr = E_FAIL;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	if (Dispersion <= 0.01f)Dispersion = 0.01f;

	ComputeGaussWeights(Dispersion);

	//定数バッファにデータを書き込む
	CBUFFER cb;
	if (SUCCEEDED(pD3DDeviceContext->Map(m_pConstantBuffers, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
	{
		memcpy(cb.Weight, mWeights, sizeof(float) * 8);
		cb.Offset.x = 16.0f / m_Width;
		cb.Offset.y = 16.0f / m_Height;
		cb.Width = m_Width;
		cb.Height = m_Height;
		memcpy_s(mappedResource.pData, mappedResource.RowPitch, (void*)(&cb), sizeof(cb));
		pD3DDeviceContext->Unmap(m_pConstantBuffers, 0);
	}

	hr = S_OK;
	return hr;
}

HRESULT GAUSSIANFILTER::Render(ID3D11Device* pD3DDevice, ID3D11DeviceContext* pD3DDeviceContext, IN ID3D11RenderTargetView* pInRTView, OUT ID3D11RenderTargetView* pOutRTView)
{
	HRESULT hr = E_FAIL;

	ID3D11Resource* pResource = NULL;
	ID3D11ShaderResourceView* pSRV = NULL;
	ID3D11Texture2D* pTexture = NULL;
	D3D11_TEXTURE2D_DESC TexDesc;
	pInRTView->GetResource(&pResource);
	pTexture = (ID3D11Texture2D*)pResource;
	pTexture->GetDesc(&TexDesc);
	SAFE_RELEASE(pResource);

	// 解像度が一致していないときはエラー
	if (m_Width != TexDesc.Width)return hr;
	if (m_Height != TexDesc.Height)return hr;
	
	// Pass0を処理
	{
		float ClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
		// 自前のレンダーターゲットビューに切り替え
		pD3DDeviceContext->OMSetRenderTargets(1, &pInRTView, nullptr);
		pD3DDeviceContext->ClearRenderTargetView(pInRTView, ClearColor);
		pD3DDeviceContext->ClearDepthStencilView(D3D.g_pDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);

		//描画シェーダを設定
		pD3DDeviceContext->VSSetShader(m_pVertexShader[0], NULL, NULL);
		//pD3DDeviceContext->GSSetShader(NULL, NULL, NULL);
		pD3DDeviceContext->PSSetShader(m_pPixelShader[0], NULL, NULL);

		// レンダリングテクスチャを設定
		pD3DDeviceContext->PSSetShaderResources(0, 1, &D3D.g_pSRV);
		// ピクセルシェーダーにサンプラステートを設定する。
		pD3DDeviceContext->PSSetSamplers(0, 1, &m_pSamplerState);


		// 描画
	    // 頂点シェーダーに定数バッファを設定する
		pD3DDeviceContext->VSSetConstantBuffers(0, 1, &m_pConstantBuffers);
		// ピクセルシェーダーに定数バッファを設定する
		pD3DDeviceContext->PSSetConstantBuffers(0, 1, &m_pConstantBuffers);
		// インプットレイアウトの設定
		pD3DDeviceContext->IASetInputLayout(m_pLayout);

		// 頂点バッファ設定
		UINT stride = sizeof(VERTEX);
		UINT offset = 0;
		pD3DDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);
		// プリミティブ タイプおよびデータの順序に関する情報を設定
		pD3DDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

		// 描画
		pD3DDeviceContext->Draw(4, 0);
	}

	// Pass1を処理
	{
		float ClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
		// 自前のレンダーターゲットビューに切り替え
		pD3DDeviceContext->OMSetRenderTargets(1, &pOutRTView, nullptr);
		pD3DDeviceContext->ClearRenderTargetView(pOutRTView, ClearColor); 
		pD3DDeviceContext->ClearDepthStencilView(D3D.g_pDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);

		//描画シェーダを設定
		pD3DDeviceContext->VSSetShader(m_pVertexShader[1], NULL, NULL);
		//pD3DDeviceContext->GSSetShader(NULL, NULL, NULL);
		pD3DDeviceContext->PSSetShader(m_pPixelShader[1], NULL, NULL);

		// レンダリングテクスチャを設定
		pD3DDeviceContext->PSSetShaderResources(0, 1, &D3D.g_pSRV);
		// ピクセルシェーダーにサンプラステートを設定する。
		pD3DDeviceContext->PSSetSamplers(0, 1, &m_pSamplerState);


		// 描画
		// 頂点シェーダーに定数バッファを設定する
		pD3DDeviceContext->VSSetConstantBuffers(0, 1, &m_pConstantBuffers);
		// ピクセルシェーダーに定数バッファを設定する
		pD3DDeviceContext->PSSetConstantBuffers(0, 1, &m_pConstantBuffers);
		// インプットレイアウトの設定
		pD3DDeviceContext->IASetInputLayout(m_pLayout);

		// 頂点バッファ設定
		UINT stride = sizeof(VERTEX);
		UINT offset = 0;
		pD3DDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);
		// プリミティブ タイプおよびデータの順序に関する情報を設定
		pD3DDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

		// 描画
		pD3DDeviceContext->Draw(4, 0);
	}

	hr = S_OK;
EXIT:

	return hr;
}

void GAUSSIANFILTER::ComputeGaussWeights(float dispersion)
{
	const UINT Division = 8;
	float total = 0.0f;
	for (int i = 0; i < Division; ++i)
	{
		float pos = 1.0f + 2.0f * (float)i;
		mWeights[i] = expf(-0.5f * (pos * pos) / dispersion);
		total += 2.0f * mWeights[i];
	}

	float invTotal = 1.0f / total;
	for (int i = 0; i < Division; ++i)
	{
		mWeights[i] *= invTotal;
	}

	/*
	// ガウス関数による重みの計算
	for (int i = 0; i < Division; i++)
	{
		float pos = (float)i * 2.0f;
		mWeights[i] = expf(-pos * pos * dispersion);
		total += mWeights[i];
	}

	// 重みの規格化
	for (int i = 0; i < Division; i++)
		mWeights[i] = mWeights[i] / total * 0.5f;
	*/
}