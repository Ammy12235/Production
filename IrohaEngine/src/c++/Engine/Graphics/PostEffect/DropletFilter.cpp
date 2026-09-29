#include "DropletFilter.h"

#include <IrohaGraphics.h>

#define _DEBUG 1

DropletFilter::DropletFilter()
{
	_frictionTextureId = TEX_FAC.CreateTexture("tex/noizenone.png");
}

DropletFilter::~DropletFilter()
{
	cleanup();
}

void DropletFilter::cleanup()
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

	SAFE_RELEASE(m_pVertexBuffer);
	SAFE_RELEASE(m_pConstantBuffer);
	SAFE_RELEASE(m_pSamplerState);

	for (int i = 0; i < numPS; i++)
	{
		SAFE_RELEASE(m_pPixelShader[i]);
	}
	SAFE_RELEASE(m_pLayout);
	SAFE_RELEASE(m_pVertexShader);
	SAFE_RELEASE(g_pDSV);
}

// 初期化
bool DropletFilter::init(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int width, int height)
{
	HRESULT hr;
	SetShaderDirectory();


	m_Width = width;
	m_Height = height;

	//デバイス類設定
	m_pD3DDevice = device;
	m_pD3DDeviceContext = deviceContext;
	// インプットレイアウトの定義
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",   0, DXGI_FORMAT_R32G32_FLOAT, 0,  D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },

	};
	UINT numElements = ARRAYSIZE(layout);
#if _DEBUG
	//頂点シェーダーを作成する
	ShaderDesc desc =
	{
		"DropletFilter.hlsl",
		 "VS",
		 "vs_5_0"
	};
	hr = SHADER_FAC.CreateVertexShader(eGenerateType::FROM_FILE, desc, &m_pVertexShader, layout, numElements, &m_pLayout);
	if (FAILED(hr))
		return false;


	//ピクセルシェーダーを作成する
	desc.shaderModel = "ps_5_0";
	for (int i = 0; i < numPS; i++)
	{
		desc.entryPointName = ps_main[i];

		hr = SHADER_FAC.CreatePixelShader(eGenerateType::FROM_FILE, desc, &m_pPixelShader[i]);
		if (FAILED(hr))
			return false;
	}
#else

	//頂点シェーダーを作成する
	ShaderDesc desc =
	{
		"132",
		 "VS",
		 "vs_5_0"
	};
	hr = SHADER_FAC.CreateVertexShader(eGenerateType::FROM_RESOURCE, desc, &m_pVertexShader, layout, numElements, &m_pLayout);
	if (FAILED(hr))
		return false;

	//ピクセルシェーダーを作成する
	desc.fileName = "131";
	//ピクセルシェーダーを作成する
	for (int i = 0; i < numPS; i++)
	{
		desc.entryPointName = ps_main[i];
		desc.shaderModel = "ps_5_0";

		hr = SHADER_FAC.CreatePixelShader(eGenerateType::FROM_RESOURCE, desc, &m_pPixelShader[i]);
		if (FAILED(hr))
			return false;
	}

#endif

	// サンプラーステートの設定
	D3D11_SAMPLER_DESC samplerDesc;

	//リニア補間
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
	hr = m_pD3DDevice->CreateSamplerState(&samplerDesc, &m_pSamplerState);
	if (FAILED(hr)) return false;

	// 射影座標系上での頂点座標を設定する
	VERTEX v[] = {
			 XMFLOAT3(-1,  -1, 0), XMFLOAT2(0, 1),
			  XMFLOAT3(-1,  1, 0),XMFLOAT2(0, 0),
			  XMFLOAT3(1, -1, 0), XMFLOAT2(1, 1),
			  XMFLOAT3(1, 1, 0),XMFLOAT2(1, 0)
	};

	ShaderUtility shaderUtility;
	//頂点バッファ、定数バッファを生成する
	hr = shaderUtility.CreateVertexBuffer(m_pD3DDevice, &m_pVertexBuffer, (void*)v, sizeof(v), D3D11_CPU_ACCESS_WRITE);
	if (FAILED(hr))
		return false;
	hr = shaderUtility.CreateConstantBuffer(m_pD3DDevice, &m_pConstantBuffer, nullptr, sizeof(CBUFFER), D3D11_CPU_ACCESS_WRITE);
	if (FAILED(hr))
		return false;

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
			hr = m_pD3DDevice->CreateTexture2D(&texDesc, NULL, &g_pDropletsTex[i]);
			if (FAILED(hr))return false;
			hr = m_pD3DDevice->CreateTexture2D(&texDesc, NULL, &g_pDropletsLocusTex[i]);
			if (FAILED(hr))return false;
			hr = m_pD3DDevice->CreateTexture2D(&texDesc, NULL, &g_pDropletsTex2[i]);
			if (FAILED(hr))return false;
		}
		hr = m_pD3DDevice->CreateTexture2D(&texDesc, NULL, &g_pBlurTex);
		if (FAILED(hr))return false;
		hr = m_pD3DDevice->CreateTexture2D(&texDesc, NULL, &g_pNormalTex);
		if (FAILED(hr))return false;
		hr = m_pD3DDevice->CreateTexture2D(&texDesc, NULL, &g_pMergeTex);
		if (FAILED(hr))return false;

		// レンダーターゲットビューの設定
		D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
		memset(&rtvDesc, 0, sizeof(rtvDesc));
		rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		for (int i = 0; i < 2; i++)
		{
			// レンダーターゲットビューの生成
			hr = m_pD3DDevice->CreateRenderTargetView(g_pDropletsTex[i], &rtvDesc, &g_pDropletsRTV[i]);
			if (FAILED(hr))return false;
			hr = m_pD3DDevice->CreateRenderTargetView(g_pDropletsLocusTex[i], &rtvDesc, &g_pDropletsLocusRTV[i]);
			if (FAILED(hr))return false;
			hr = m_pD3DDevice->CreateRenderTargetView(g_pDropletsTex2[i], &rtvDesc, &g_pDropletsRTV2[i]);
			if (FAILED(hr))return false;
		}
		hr = m_pD3DDevice->CreateRenderTargetView(g_pBlurTex, &rtvDesc, &g_pBlurRTV);
		if (FAILED(hr))return false;
		hr = m_pD3DDevice->CreateRenderTargetView(g_pNormalTex, &rtvDesc, &g_pNormalRTV);
		if (FAILED(hr))return false;
		hr = m_pD3DDevice->CreateRenderTargetView(g_pMergeTex, &rtvDesc, &g_pMergeRTV);
		if (FAILED(hr))return false;

		// シェーダリソースビューの設定
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		memset(&srvDesc, 0, sizeof(srvDesc));
		srvDesc.Format = rtvDesc.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = 1;
		for (int i = 0; i < 2; i++)
		{
			// シェーダリソースビューの生成
			hr = m_pD3DDevice->CreateShaderResourceView(g_pDropletsTex[i], &srvDesc, &g_pDropletsSRV[i]);
			if (FAILED(hr))	return false;
			hr = m_pD3DDevice->CreateShaderResourceView(g_pDropletsLocusTex[i], &srvDesc, &g_pDropletsLocusSRV[i]);
			if (FAILED(hr))	return false;
			hr = m_pD3DDevice->CreateShaderResourceView(g_pDropletsTex2[i], &srvDesc, &g_pDropletsSRV2[i]);
			if (FAILED(hr))	return false;
		}
		hr = m_pD3DDevice->CreateShaderResourceView(g_pBlurTex, &srvDesc, &g_pBlurSRV);
		if (FAILED(hr))	return false;
		hr = m_pD3DDevice->CreateShaderResourceView(g_pNormalTex, &srvDesc, &g_pNormalSRV);
		if (FAILED(hr))	return false;
		hr = m_pD3DDevice->CreateShaderResourceView(g_pMergeTex, &srvDesc, &g_pMergeSRV);
		if (FAILED(hr))	return false;


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

		hr = m_pD3DDevice->CreateTexture2D(&Tex2DDesc, NULL, &pTex2D);
		if (FAILED(hr))
		{
			SAFE_RELEASE(pTex2D);
			return false;
		}
		hr = m_pD3DDevice->CreateDepthStencilView(pTex2D, &DescDS, &g_pDSV);
		if (FAILED(hr))
		{

			SAFE_RELEASE(pTex2D);
			return false;

		}

		SAFE_RELEASE(pTex2D);
	}
	//ビューポートを設定
	g_pVp.Width = m_Width;
	g_pVp.Height = m_Height;
	g_pVp.MinDepth = 0.0f;
	g_pVp.MaxDepth = 1.0f;
	g_pVp.TopLeftX = 0;
	g_pVp.TopLeftY = 0;
	return true;
}

void DropletFilter::apply(IN ID3D11RenderTargetView* inputRTV, OUT ID3D11RenderTargetView* outputRTV)
{

	setConstantBuffers(_distortion, _attenuate, _addDynamicDropletPos, _addStaticDropletPos, _isAddDynamic, _isAddStatic);
	ShaderUtility shaderUtility;

	//ID3D11ShaderResourceView* pSRV = NULL;
	D3D11_VIEWPORT OldViewport;
	// ビューポートを退避
	UINT pNumVierports = 1;
	m_pD3DDeviceContext->RSGetViewports(&pNumVierports, &OldViewport);
	// ビューポートのサイズを変更する
	//高速化のため初期化の段階で解像度を半分にしている
	m_pD3DDeviceContext->RSSetViewports(1, &g_pVp);
	m_pRTVTargetIndex = 1 - m_pRTVTargetIndex;

	// Pass0(Step1)を処理（水滴マップの更新。水滴を追加するかを決定するのはこのパス）
	{
		//=============================================
		//動的な水滴の更新
		//=============================================

		ID3D11RenderTargetView* renderTargetViews[2]//使用するRTVを列挙する
			= { g_pDropletsRTV[1 - m_pRTVTargetIndex] ,g_pDropletsLocusRTV[1 - m_pRTVTargetIndex] };
		// 自前のレンダーターゲットビューに切り替え
		m_pD3DDeviceContext->OMSetRenderTargets(2, renderTargetViews, nullptr);//二つのテクスチャを出力するためRTVは二つ

		float ClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
		for (int i = 0; i < 2; i++)
			m_pD3DDeviceContext->ClearRenderTargetView(renderTargetViews[i], ClearColor);
		m_pD3DDeviceContext->ClearDepthStencilView(g_pDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);

		//描画シェーダを設定する
		m_pD3DDeviceContext->VSSetShader(m_pVertexShader, NULL, NULL);
		//m_pD3DDeviceContext->GSSetShader(NULL, NULL, NULL);
		if (_isAddDynamic)
			m_pD3DDeviceContext->PSSetShader(m_pPixelShader[PS_AddDynamicDroplets], NULL, NULL);//追加する
		else
			m_pD3DDeviceContext->PSSetShader(m_pPixelShader[PS_NoneAddDynamicDroplets], NULL, NULL);//追加しない

		// ピクセルシェーダーにサンプラステートを設定する
		m_pD3DDeviceContext->PSSetSamplers(0, 1, &m_pSamplerState);

		//水滴マップをスロット０、水滴軌跡マップをスロット１にセットする
		m_pD3DDeviceContext->PSSetShaderResources(0, 1, &g_pDropletsSRV[m_pRTVTargetIndex]);
		m_pD3DDeviceContext->PSSetShaderResources(1, 1, &g_pDropletsLocusSRV[m_pRTVTargetIndex]);

		//摩擦ノイズマップをスロット２にセットする
		m_pD3DDeviceContext->PSSetShaderResources(2, 1, TEX_FAC.getTexturebyId(_frictionTextureId)->getTexResource());

		// 頂点シェーダーに定数バッファを設定する
		m_pD3DDeviceContext->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);
		// ピクセルシェーダーに定数バッファを設定する
		m_pD3DDeviceContext->PSSetConstantBuffers(0, 1, &m_pConstantBuffer);
		// インプットレイアウトの設定
		m_pD3DDeviceContext->IASetInputLayout(m_pLayout);

		// 頂点バッファ設定
		UINT stride = sizeof(VERTEX);
		UINT offset = 0;
		m_pD3DDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);
		// プリミティブ タイプおよびデータの順序に関する情報を設定
		m_pD3DDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

		// 描画
		m_pD3DDeviceContext->Draw(4, 0);
		//=============================================
		//静的な水滴を更新
		//=============================================

		// 自前のレンダーターゲットビューに切り替え
		m_pD3DDeviceContext->OMSetRenderTargets(1, &g_pDropletsRTV2[1 - m_pRTVTargetIndex], nullptr);

		m_pD3DDeviceContext->ClearRenderTargetView(g_pDropletsRTV2[1 - m_pRTVTargetIndex], ClearColor);
		m_pD3DDeviceContext->ClearDepthStencilView(g_pDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);

		//描画シェーダを設定する
		m_pD3DDeviceContext->VSSetShader(m_pVertexShader, NULL, NULL);
		//m_pD3DDeviceContext->GSSetShader(NULL, NULL, NULL);

		if (_isAddStatic)
			m_pD3DDeviceContext->PSSetShader(m_pPixelShader[PS_AddStaticDroplets], NULL, NULL);//追加する
		else
			m_pD3DDeviceContext->PSSetShader(m_pPixelShader[PS_NoneAddStaticDroplets], NULL, NULL);//追加しない

		//m_pD3DDeviceContext->PSSetShader(m_pPixelShader[PS_NoneAddStaticDroplets], NULL, NULL);//追加しない

		// ピクセルシェーダーにサンプラステートを設定する
		m_pD3DDeviceContext->PSSetSamplers(0, 1, &m_pSamplerState);

		//水滴マップをスロット０にセットする
		m_pD3DDeviceContext->PSSetShaderResources(0, 1, &g_pDropletsSRV2[m_pRTVTargetIndex]);


		// 描画
		m_pD3DDeviceContext->Draw(4, 0);


		//=============================================
		//合成
		//=============================================

		// 自前のレンダーターゲットビューに切り替え
		m_pD3DDeviceContext->OMSetRenderTargets(1, &g_pMergeRTV, nullptr);


		m_pD3DDeviceContext->ClearRenderTargetView(g_pMergeRTV, ClearColor);
		m_pD3DDeviceContext->ClearDepthStencilView(g_pDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);

		//描画シェーダを設定する
		m_pD3DDeviceContext->VSSetShader(m_pVertexShader, NULL, NULL);
		//m_pD3DDeviceContext->GSSetShader(NULL, NULL, NULL);
		m_pD3DDeviceContext->PSSetShader(m_pPixelShader[PS_MergeDroplets], NULL, NULL);


		// ピクセルシェーダーにサンプラステートを設定する
		m_pD3DDeviceContext->PSSetSamplers(0, 1, &m_pSamplerState);

		//水滴マップ（動的）をスロット０、水滴マップ(静的)をスロット１にセットする
		m_pD3DDeviceContext->PSSetShaderResources(0, 1, &g_pDropletsLocusSRV[m_pRTVTargetIndex]);
		m_pD3DDeviceContext->PSSetShaderResources(1, 1, &g_pDropletsSRV2[m_pRTVTargetIndex]);

		// 描画
		m_pD3DDeviceContext->Draw(4, 0);

	}
	//Pass1(Step2)を処理（ブラーの適応）
	{

		float ClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
		// 自前のレンダーターゲットビューに切り替え
		m_pD3DDeviceContext->OMSetRenderTargets(1, &g_pBlurRTV, nullptr);
		m_pD3DDeviceContext->ClearRenderTargetView(g_pBlurRTV, ClearColor);
		m_pD3DDeviceContext->ClearDepthStencilView(g_pDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);

		m_pD3DDeviceContext->PSSetShader(m_pPixelShader[PS_Blur], NULL, NULL);//ブラーのPSをセット

		//水滴の軌跡マップをセットする
		m_pD3DDeviceContext->PSSetShaderResources(0, 1, &g_pMergeSRV);

		// 描画
		m_pD3DDeviceContext->Draw(4, 0);
	}

	//Pass2(Step3)を処理（法線マップの作成）
	{
		float ClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
		// 自前のレンダーターゲットビューに切り替え

		m_pD3DDeviceContext->OMSetRenderTargets(1, &g_pNormalRTV, nullptr);
		m_pD3DDeviceContext->ClearRenderTargetView(g_pNormalRTV, ClearColor);
		m_pD3DDeviceContext->ClearDepthStencilView(g_pDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);

		m_pD3DDeviceContext->PSSetShader(m_pPixelShader[PS_CreateNormalMap], NULL, NULL);//法線マップ作成のPSをセット

		//ブラーされた水滴の軌跡マップをセットする
		m_pD3DDeviceContext->PSSetShaderResources(0, 1, &g_pBlurSRV);

		// 描画
		m_pD3DDeviceContext->Draw(4, 0);
	}
	m_pD3DDeviceContext->RSSetViewports(1, &OldViewport);
	//Pass3(Step4)を処理（バックバッファをゆがませる）
	{
		float ClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
		// 自前のレンダーターゲットビューに切り替え
		m_pD3DDeviceContext->OMSetRenderTargets(1, &outputRTV, nullptr);
		m_pD3DDeviceContext->ClearRenderTargetView(outputRTV, ClearColor);
		m_pD3DDeviceContext->ClearDepthStencilView(g_pDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);

		m_pD3DDeviceContext->PSSetShader(m_pPixelShader[PS_Distortion], NULL, NULL);//ゆがみのPSをセット

		//法線マップをセットする
		m_pD3DDeviceContext->PSSetShaderResources(0, 1, &g_pNormalSRV);
		ID3D11ShaderResourceView* pSRV;
		ShaderUtility shaderUtility;
		pSRV = shaderUtility.GetSRViewFromRTView(m_pD3DDevice, inputRTV);
		//バックバッファービューをセットする
		m_pD3DDeviceContext->PSSetShaderResources(1, 1, &pSRV);
		// 描画
		m_pD3DDeviceContext->Draw(4, 0);

		SAFE_RELEASE(pSRV);
	}

	ID3D11ShaderResourceView* null[] = { nullptr ,nullptr ,nullptr };
	m_pD3DDeviceContext->PSSetShaderResources(0, 3, null);

}

HRESULT DropletFilter::setConstantBuffers(float distortion, float attenuate, XMFLOAT2 addDynamicDropletPos, XMFLOAT2 addStaticDropletPos, bool isAddDynamic, bool isAddStatic)
{
	HRESULT hr = E_FAIL;
	D3D11_MAPPED_SUBRESOURCE mappedResource;

	//定数バッファにデータを書き込む
	CBUFFER cb = {};
	if (SUCCEEDED(m_pD3DDeviceContext->Map(m_pConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
	{
		cb.offset.x = (float)1 / m_Width;
		cb.offset.y = (float)1 / m_Height;
		if (isAddDynamic)
		{
			cb.addDynamicDropletPos.x = addDynamicDropletPos.x;
			cb.addDynamicDropletPos.y = addDynamicDropletPos.y;
		}
		if (isAddStatic)
		{
			cb.addStaticDropletPos.x = addStaticDropletPos.x;
			cb.addStaticDropletPos.y = addStaticDropletPos.y;
		}
		cb.distortion = distortion;
		cb.attenuate = attenuate;
		memcpy_s(mappedResource.pData, mappedResource.RowPitch, (void*)(&cb), sizeof(cb));
		m_pD3DDeviceContext->Unmap(m_pConstantBuffer, 0);
	}

	hr = S_OK;
	return hr;
}

void DropletFilter::setParameter(float distortion, float attenuate, XMFLOAT2 addDynamicDropletPos, XMFLOAT2 addStaticDropletPos, bool isAddDynamic, bool isAddStatic)
{

	if (distortion <= 0.0f)distortion = 0.0f;
	if (attenuate <= 0.001f)attenuate = 0.001f;

	_distortion = distortion;
	_attenuate = attenuate;

	_isAddDynamic = isAddDynamic;//レンダリングするときに水滴を追加するかの判断に使用する
	_isAddStatic = isAddStatic;

	_addDynamicDropletPos = addDynamicDropletPos;
	_addStaticDropletPos = addStaticDropletPos;


}
