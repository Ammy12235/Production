#include "DropWaterFilter.h"
#include "DEFINE.h"
#include "Base.h"

DROPWATERFILTER::DROPWATERFILTER()
{
	m_pConstantBuffers = NULL;
	m_pSamplerState = NULL;
	m_Width = 0;
	m_Height = 0;
}

DROPWATERFILTER::~DROPWATERFILTER()
{
	RemoveDevice();
}

void DROPWATERFILTER::RemoveDevice()
{

	SAFE_RELEASE(g_pMergeSRV);
	SAFE_RELEASE(g_pNormalSRV);
	SAFE_RELEASE(g_pBlurSRV);
	for (int i = 0; i < 2; i++)
		SAFE_RELEASE(g_pDropletsLocusSRV[i]);
	for (int i = 0; i < 2; i++)
		SAFE_RELEASE(g_pDropletsSRV[i]);
	for (int i = 0; i < 2; i++)
		SAFE_RELEASE(g_pDropletsSRV2[i]);

	SAFE_RELEASE(g_pMergeTex);
	SAFE_RELEASE(g_pNormalTex);
	SAFE_RELEASE(g_pBlurTex);
	for (int i = 0; i < 2; i++)
		SAFE_RELEASE(g_pDropletsLocusTex[i]);
	for (int i = 0; i < 2; i++)
		SAFE_RELEASE(g_pDropletsTex[i]);
	for (int i = 0; i < 2; i++)
		SAFE_RELEASE(g_pDropletsTex2[i]);

	SAFE_RELEASE(g_pMergeRTV);
	SAFE_RELEASE(g_pNormalRTV);
	SAFE_RELEASE(g_pBlurRTV);
	for (int i = 0; i < 2; i++)
		SAFE_RELEASE(g_pDropletsLocusRTV[i]);
	for (int i = 0; i < 2; i++)
		SAFE_RELEASE(g_pDropletsRTV[i]);
	for (int i = 0; i < 2; i++)
		SAFE_RELEASE(g_pDropletsRTV2[i]);

	SAFE_RELEASE(g_pDSV);
	SAFE_RELEASE(m_pVertexBuffer);
	SAFE_RELEASE(m_pSamplerState);
	SAFE_RELEASE(m_pConstantBuffers);
	for (int i = 0; i < numPS; i++)
		SAFE_RELEASE(m_pPixelShader[i]);
	SAFE_RELEASE(m_pLayout);
	SAFE_RELEASE(m_pVertexShader);
}

//シェーダファイルをコンパイルする
HRESULT DROPWATERFILTER::CompileShaderFromFile(const WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut)
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
HRESULT DROPWATERFILTER::Init(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dDeviceContext, TCHAR pSrcFile[], UINT Width, UINT Height)
{
	HRESULT hr = E_FAIL;

	ID3D11Texture2D* pTex2D = NULL;
	ID3D10Blob* pVSBlob = NULL, * pPSBlob = NULL;

	m_Width = Width;
	m_Height = Height;
	pD3DDevice = pd3dDevice;
	pD3DDeviceContext = pd3dDeviceContext;

	SetShaderDirectory();

	// バーテックスシェーダのコンパイル
	hr = CompileShaderFromFile(pSrcFile, vs_main, "vs_5_0", &pVSBlob);
	if (FAILED(hr))
	{
		MessageBox(nullptr,
			L"HLSLファイル(DropWaterFilter)をコンパイルできません。HLSLファイルを含むディレクトリからこの実行可能ファイルを実行してください。", L"Error", MB_OK);
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

	//============================================
	// ピクセルシェーダのコンパイル
	// １、水滴追加（動的）
	// ２、水滴追加しない（動的）
	// ３、水滴追加（静的）
	// ４、水滴追加しない（静的）
	// ５、ブラー
	// ６、法線マップ
	// ７、ゆがみ
	//============================================
	for (int i = 0; i < numPS; i++)
	{


		hr = CompileShaderFromFile(pSrcFile, ps_main[i], "ps_5_0", &pPSBlob);
		if (FAILED(hr))
		{
			MessageBox(nullptr,
				L"HLSLファイル(DropWaterFilter)をコンパイルできません。HLSLファイルを含むディレクトリからこの実行可能ファイルを実行してください。", L"Error", MB_OK);
			return hr;
		}

		// ピクセルシェーダの作成
		hr = pD3DDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &m_pPixelShader[i]);
		if (FAILED(hr))
		{

			return hr;
		}
	}
	pPSBlob->Release();
	pPSBlob = nullptr;

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


	// レンダーターゲットビュー、テクスチャを作成
	{
		// 2次元テクスチャの設定
		D3D11_TEXTURE2D_DESC texDesc;
		memset(&texDesc, 0, sizeof(texDesc));
		texDesc.Usage = D3D11_USAGE_DEFAULT;
		texDesc.Format = DXGI_FORMAT_R8G8B8A8_TYPELESS;
		texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		texDesc.Width = m_Width;
		texDesc.Height = m_Height;
		texDesc.CPUAccessFlags = 0;
		texDesc.MipLevels = 1;
		texDesc.ArraySize = 1;
		texDesc.SampleDesc.Count = 1;
		texDesc.SampleDesc.Quality = 0;

		// 2次元テクスチャの生成
		for (int i = 0; i < 2; i++)
		{
			hr = pD3DDevice->CreateTexture2D(&texDesc, NULL, &g_pDropletsTex[i]);
			if (FAILED(hr))return hr;
			hr = pD3DDevice->CreateTexture2D(&texDesc, NULL, &g_pDropletsLocusTex[i]);
			if (FAILED(hr))return hr;
			hr = pD3DDevice->CreateTexture2D(&texDesc, NULL, &g_pDropletsTex2[i]);
			if (FAILED(hr))return hr;
		}
		hr = pD3DDevice->CreateTexture2D(&texDesc, NULL, &g_pBlurTex);
		if (FAILED(hr))return hr;
		hr = pD3DDevice->CreateTexture2D(&texDesc, NULL, &g_pNormalTex);
		if (FAILED(hr))return hr;
		hr = pD3DDevice->CreateTexture2D(&texDesc, NULL, &g_pMergeTex);
		if (FAILED(hr))return hr;

		// レンダーターゲットビューの設定
		D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
		memset(&rtvDesc, 0, sizeof(rtvDesc));
		rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		for (int i = 0; i < 2; i++)
		{
			// レンダーターゲットビューの生成
			hr = pD3DDevice->CreateRenderTargetView(g_pDropletsTex[i], &rtvDesc, &g_pDropletsRTV[i]);
			if (FAILED(hr))return hr;
			hr = pD3DDevice->CreateRenderTargetView(g_pDropletsLocusTex[i], &rtvDesc, &g_pDropletsLocusRTV[i]);
			if (FAILED(hr))return hr;
			hr = pD3DDevice->CreateRenderTargetView(g_pDropletsTex2[i], &rtvDesc, &g_pDropletsRTV2[i]);
			if (FAILED(hr))return hr;
		}
		hr = pD3DDevice->CreateRenderTargetView(g_pBlurTex, &rtvDesc, &g_pBlurRTV);
		if (FAILED(hr))return hr;
		hr = pD3DDevice->CreateRenderTargetView(g_pNormalTex, &rtvDesc, &g_pNormalRTV);
		if (FAILED(hr))return hr;
		hr = pD3DDevice->CreateRenderTargetView(g_pMergeTex, &rtvDesc, &g_pMergeRTV);
		if (FAILED(hr))return hr;

		// シェーダリソースビューの設定
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		memset(&srvDesc, 0, sizeof(srvDesc));
		srvDesc.Format = rtvDesc.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = 1;
		for (int i = 0; i < 2; i++)
		{
			// シェーダリソースビューの生成
			hr = pD3DDevice->CreateShaderResourceView(g_pDropletsTex[i], &srvDesc, &g_pDropletsSRV[i]);
			if (FAILED(hr))	return hr;
			hr = pD3DDevice->CreateShaderResourceView(g_pDropletsLocusTex[i], &srvDesc, &g_pDropletsLocusSRV[i]);
			if (FAILED(hr))	return hr;
			hr = pD3DDevice->CreateShaderResourceView(g_pDropletsTex2[i], &srvDesc, &g_pDropletsSRV2[i]);
			if (FAILED(hr))	return hr;
		}
		hr = pD3DDevice->CreateShaderResourceView(g_pBlurTex, &srvDesc, &g_pBlurSRV);
		if (FAILED(hr))	return hr;
		hr = pD3DDevice->CreateShaderResourceView(g_pNormalTex, &srvDesc, &g_pNormalSRV);
		if (FAILED(hr))	return hr;
		hr = pD3DDevice->CreateShaderResourceView(g_pMergeTex, &srvDesc, &g_pMergeSRV);
		if (FAILED(hr))	return hr;


	}

	// 深度ステンシルビューを作成
	{
		ID3D11Texture2D* pTex2D;
		D3D11_TEXTURE2D_DESC Tex2DDesc;

		::ZeroMemory(&Tex2DDesc, sizeof(D3D11_TEXTURE2D_DESC));
		Tex2DDesc.ArraySize = 1;
		Tex2DDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		Tex2DDesc.Usage = D3D11_USAGE_DEFAULT;
		Tex2DDesc.Format = DXGI_FORMAT_D32_FLOAT;
		Tex2DDesc.Width = m_Width;
		Tex2DDesc.Height = m_Height;
		Tex2DDesc.MipLevels = 1;
		Tex2DDesc.SampleDesc.Count = 1;

		D3D11_DEPTH_STENCIL_VIEW_DESC DescDS;
		::ZeroMemory(&DescDS, sizeof(DescDS));
		DescDS.Format = Tex2DDesc.Format;
		DescDS.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

		hr = pD3DDevice->CreateTexture2D(&Tex2DDesc, NULL, &pTex2D);
		if (FAILED(hr)) goto EXIT;
		hr = pD3DDevice->CreateDepthStencilView(pTex2D, &DescDS, &g_pDSV);
		if (FAILED(hr)) goto EXIT;

		SAFE_RELEASE(pTex2D);
	}

	//ビューポートを設定
	g_pVp.Width = m_Width;
	g_pVp.Height = m_Height;
	g_pVp.MinDepth = 0.0f;
	g_pVp.MaxDepth = 1.0f;
	g_pVp.TopLeftX = 0;
	g_pVp.TopLeftY = 0;

	//pD3DDeviceContext->RSSetViewports(1, &g_pVp);

EXIT:
	SAFE_RELEASE(pTex2D);

	return hr;
}

// レンダーターゲットビューからシェーダーリソースビューを作成する
ID3D11ShaderResourceView* DROPWATERFILTER::GetSRViewFromRTView(ID3D11Device* pD3DDevice, ID3D11RenderTargetView* pRTView) const
{
	ID3D11Resource* pResource = NULL;
	ID3D11ShaderResourceView* pSRView = NULL;

	if (pRTView == nullptr)
		return pSRView;

	pRTView->GetResource(&pResource);
	pD3DDevice->CreateShaderResourceView(pResource, NULL, &pSRView);
	SAFE_RELEASE(pResource);

	return pSRView;
}

// 定数バッファを設定する
HRESULT DROPWATERFILTER::SetConstantBuffers(float distortion, float attenuate, XMFLOAT2 addDynamicDropletPos, XMFLOAT2 addStaticDropletPos, bool isAddDynamic, bool isAddStatic)
{

	HRESULT hr = E_FAIL;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	if (distortion <= 0.0f)distortion = 0.0f;
	if (attenuate <= 0.001f)attenuate = 0.001f;

	isAddDynamicDropletWater = isAddDynamic;//レンダリングするときに水滴を追加するかの判断に使用する
	isAddStaticDropletWater = isAddStatic;

	//定数バッファにデータを書き込む
	CBUFFER cb;
	if (SUCCEEDED(pD3DDeviceContext->Map(m_pConstantBuffers, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
	{
		cb.offset.x = (float)1 / m_Width;
		cb.offset.y = (float)1 / m_Height;
		if (isAddDynamicDropletWater)
		{
			cb.addDynamicDropletPos.x = addDynamicDropletPos.x;
			cb.addDynamicDropletPos.y = addDynamicDropletPos.y;
		}
		if (isAddStaticDropletWater)
		{
			cb.addStaticDropletPos.x = addStaticDropletPos.x;
			cb.addStaticDropletPos.y = addStaticDropletPos.y;
		}
		cb.distortion = distortion;
		cb.attenuate = attenuate;
		memcpy_s(mappedResource.pData, mappedResource.RowPitch, (void*)(&cb), sizeof(cb));
		pD3DDeviceContext->Unmap(m_pConstantBuffers, 0);
	}

	hr = S_OK;
	return hr;
}

HRESULT DROPWATERFILTER::Render(IN  ID3D11RenderTargetView* pInRTView, OUT ID3D11RenderTargetView* pOutRTView)
{
	HRESULT hr = E_FAIL;

	m_pRTVTargetIndex = 1 - m_pRTVTargetIndex;
	pD3DDeviceContext->RSSetViewports(1, &g_pVp);
	// Pass0(Step1)を処理（水滴マップの更新。水滴を追加するかを決定するのはこのパス）
	{
		//=============================================
		//動的な水滴の更新
		//=============================================

		ID3D11RenderTargetView* renderTargetViews[2]//使用するRTVを列挙する
			= { g_pDropletsRTV[1 - m_pRTVTargetIndex] ,g_pDropletsLocusRTV[1 - m_pRTVTargetIndex] };
		// 自前のレンダーターゲットビューに切り替え
		pD3DDeviceContext->OMSetRenderTargets(2, renderTargetViews, nullptr);//二つのテクスチャを出力するためRTVは二つ

		float ClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
		for (int i = 0; i < 2; i++)
			pD3DDeviceContext->ClearRenderTargetView(renderTargetViews[i], ClearColor);
		pD3DDeviceContext->ClearDepthStencilView(g_pDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);

		//描画シェーダを設定する
		pD3DDeviceContext->VSSetShader(m_pVertexShader, NULL, NULL);
		//pD3DDeviceContext->GSSetShader(NULL, NULL, NULL);
		if (isAddDynamicDropletWater)
			pD3DDeviceContext->PSSetShader(m_pPixelShader[PS_AddDynamicDroplets], NULL, NULL);//追加する
		else
			pD3DDeviceContext->PSSetShader(m_pPixelShader[PS_NoneAddDynamicDroplets], NULL, NULL);//追加しない

		// ピクセルシェーダーにサンプラステートを設定する
		pD3DDeviceContext->PSSetSamplers(0, 1, &m_pSamplerState);

		//水滴マップをスロット０、水滴軌跡マップをスロット１にセットする
		pD3DDeviceContext->PSSetShaderResources(0, 1, &g_pDropletsSRV[m_pRTVTargetIndex]);
		pD3DDeviceContext->PSSetShaderResources(1, 1, &g_pDropletsLocusSRV[m_pRTVTargetIndex]);

		//摩擦ノイズマップをスロット２にセットする
		pD3DDeviceContext->PSSetShaderResources(2, 1, TEX_FAC.getTexture("FrictionNoize.png")->m_srv.GetAddressOf());


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

		//=============================================
		//静的な水滴を更新
		//=============================================

		// 自前のレンダーターゲットビューに切り替え
		pD3DDeviceContext->OMSetRenderTargets(1, &g_pDropletsRTV2[1 - m_pRTVTargetIndex], nullptr);

		pD3DDeviceContext->ClearRenderTargetView(g_pDropletsRTV2[1 - m_pRTVTargetIndex], ClearColor);
		pD3DDeviceContext->ClearDepthStencilView(g_pDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);

		//描画シェーダを設定する
		pD3DDeviceContext->VSSetShader(m_pVertexShader, NULL, NULL);
		//pD3DDeviceContext->GSSetShader(NULL, NULL, NULL);

		if (isAddStaticDropletWater)
			pD3DDeviceContext->PSSetShader(m_pPixelShader[PS_AddStaticDroplets], NULL, NULL);//追加する
		else
			pD3DDeviceContext->PSSetShader(m_pPixelShader[PS_NoneAddStaticDroplets], NULL, NULL);//追加しない

		//pD3DDeviceContext->PSSetShader(m_pPixelShader[PS_NoneAddStaticDroplets], NULL, NULL);//追加しない

		// ピクセルシェーダーにサンプラステートを設定する
		pD3DDeviceContext->PSSetSamplers(0, 1, &m_pSamplerState);

		//水滴マップをスロット０にセットする
		pD3DDeviceContext->PSSetShaderResources(0, 1, &g_pDropletsSRV2[m_pRTVTargetIndex]);


		// 描画
		pD3DDeviceContext->Draw(4, 0);


		//=============================================
		//合成
		//=============================================

		// 自前のレンダーターゲットビューに切り替え
		pD3DDeviceContext->OMSetRenderTargets(1, &g_pMergeRTV, nullptr);


		pD3DDeviceContext->ClearRenderTargetView(g_pMergeRTV, ClearColor);
		pD3DDeviceContext->ClearDepthStencilView(g_pDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);

		//描画シェーダを設定する
		pD3DDeviceContext->VSSetShader(m_pVertexShader, NULL, NULL);
		//pD3DDeviceContext->GSSetShader(NULL, NULL, NULL);
		pD3DDeviceContext->PSSetShader(m_pPixelShader[PS_MergeDroplets], NULL, NULL);


		// ピクセルシェーダーにサンプラステートを設定する
		pD3DDeviceContext->PSSetSamplers(0, 1, &m_pSamplerState);

		//水滴マップ（動的）をスロット０、水滴マップ(静的)をスロット１にセットする
		pD3DDeviceContext->PSSetShaderResources(0, 1, &g_pDropletsLocusSRV[m_pRTVTargetIndex]);
		pD3DDeviceContext->PSSetShaderResources(1, 1, &g_pDropletsSRV2[m_pRTVTargetIndex]);

		// 描画
		pD3DDeviceContext->Draw(4, 0);

	}
	//Pass1(Step2)を処理（ブラーの適応）
	{

		float ClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
		// 自前のレンダーターゲットビューに切り替え
		pD3DDeviceContext->OMSetRenderTargets(1, &g_pBlurRTV, nullptr);
		pD3DDeviceContext->ClearRenderTargetView(g_pBlurRTV, ClearColor);
		pD3DDeviceContext->ClearDepthStencilView(g_pDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);

		pD3DDeviceContext->PSSetShader(m_pPixelShader[PS_Blur], NULL, NULL);//ブラーのPSをセット

		//水滴の軌跡マップをセットする
		pD3DDeviceContext->PSSetShaderResources(0, 1, &g_pMergeSRV);

		// 描画
		pD3DDeviceContext->Draw(4, 0);
	}

	//Pass2(Step3)を処理（法線マップの作成）
	{
		float ClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
		// 自前のレンダーターゲットビューに切り替え

		pD3DDeviceContext->OMSetRenderTargets(1, &g_pNormalRTV, nullptr);
		pD3DDeviceContext->ClearRenderTargetView(g_pNormalRTV, ClearColor);
		pD3DDeviceContext->ClearDepthStencilView(g_pDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);

		pD3DDeviceContext->PSSetShader(m_pPixelShader[PS_CreateNormalMap], NULL, NULL);//法線マップ作成のPSをセット

		//ブラーされた水滴の軌跡マップをセットする
		pD3DDeviceContext->PSSetShaderResources(0, 1, &g_pBlurSRV);

		// 描画
		pD3DDeviceContext->Draw(4, 0);
	}
	pD3DDeviceContext->RSSetViewports(1, &D3D.vp[0]);
	//Pass3(Step4)を処理（バックバッファをゆがませる）
	{
		float ClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
		// 自前のレンダーターゲットビューに切り替え
		pD3DDeviceContext->OMSetRenderTargets(1, &pOutRTView, nullptr);
		pD3DDeviceContext->ClearRenderTargetView(pOutRTView, ClearColor);
		pD3DDeviceContext->ClearDepthStencilView(g_pDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);

		pD3DDeviceContext->PSSetShader(m_pPixelShader[PS_Distortion], NULL, NULL);//ゆがみのPSをセット

		//法線マップをセットする
		pD3DDeviceContext->PSSetShaderResources(0, 1, &g_pNormalSRV);
		ID3D11ShaderResourceView* pSRV;
		pSRV = GetSRViewFromRTView(pD3DDevice, pInRTView);
		//バックバッファービューをセットする
		pD3DDeviceContext->PSSetShaderResources(1, 1, &pSRV);
		// 描画
		pD3DDeviceContext->Draw(4, 0);

		SAFE_RELEASE(pSRV);
	}

	ID3D11ShaderResourceView* null[] = { nullptr ,nullptr };
	pD3DDeviceContext->PSSetShaderResources(0, 2, null);


	hr = S_OK;
EXIT:

	return hr;
}
