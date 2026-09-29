#include "DIRECT3D11.h"

void DIRECT3D11::RemoveDevice()
{
	SetFullScreen(false);
	SAFE_DELETE(m_pDropletFilter)
		SAFE_DELETE(m_pDiskBlurFilter);
	SAFE_DELETE(m_pGaussianBlurFilter);
	SAFE_DELETE(m_pHSVFilter);
	SAFE_DELETE(m_pPostEffectChain);
	SAFE_DELETE(m_pDrawStorage);
	SAFE_DELETE(m_pInstancing);
	SAFE_DELETE(m_pBaseShading);

	if (g_pImmediateContext)
	{
		g_pImmediateContext->ClearState();
		g_pImmediateContext->Flush();
	}
	SAFE_RELEASE(m_pBlendState);
	SAFE_RELEASE(m_pDepthStencilView);
	SAFE_RELEASE(m_pDepthStencilTexture);
	SAFE_RELEASE(g_pBackBufferRTV);
	SAFE_RELEASE(g_pInputRTV);
	SAFE_RELEASE(g_pSwapChain1);
	SAFE_RELEASE(g_pSwapChain);
	SAFE_RELEASE(g_pImmediateContext1);
	SAFE_RELEASE(g_pImmediateContext);
	SAFE_RELEASE(g_pd3dDevice1);
	SAFE_RELEASE(g_pd3dDevice);

}

//DirectXを初期化する
HRESULT DIRECT3D11::Init(D3D_INIT* pcd)
{

	HRESULT hr = S_OK;

	Log("Direc3Dの初期化を開始\n");
	/*
	RECT rc;
	GetClientRect(WINDOW::m_hWnd, &rc);//画面の幅、高さを取得、格納する
	UINT width = rc.right - rc.left;
	UINT height = rc.bottom - rc.top;
	*/
	// ウィンドウのクライアントサイズを設定
	RECT rcWnd, rcClient;
	GetWindowRect(WINDOW::m_hWnd, &rcWnd);
	GetClientRect(WINDOW::m_hWnd, &rcClient);
	float width = (rcWnd.right - rcWnd.left) - (rcClient.right - rcClient.left) + Define::WIN_W;
	float height = (rcWnd.bottom - rcWnd.top) - (rcClient.bottom - rcClient.top) + Define::WIN_H;
	SetWindowPos(WINDOW::m_hWnd, NULL, 0, 0, width, height, SWP_NOMOVE | SWP_NOZORDER);

	viewWidth = width;
	viewHeight = height;

	UINT createDeviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;//DirectX11上でDirect2Dを使用するために必要
#ifdef _DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;//デバッグモードフラグ（詳細な情報を見ることができる）
#endif

	D3D_DRIVER_TYPE driverTypes[] =//ハードウェア、WARP、リファレンスの順
	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE,
	};
	UINT numDriverTypes = ARRAYSIZE(driverTypes);

	D3D_FEATURE_LEVEL featureLevels[] =//機能レベル
	{

		D3D_FEATURE_LEVEL_11_1,	// Direct3D 11.1  ShaderModel 5
		D3D_FEATURE_LEVEL_11_0,	// Direct3D 11    ShaderModel 5
		D3D_FEATURE_LEVEL_10_1,	// Direct3D 10.1  ShaderModel 4
		D3D_FEATURE_LEVEL_10_0,	// Direct3D 10.0  ShaderModel 4
		D3D_FEATURE_LEVEL_9_3,	// Direct3D 9.3   ShaderModel 3
		D3D_FEATURE_LEVEL_9_2,	// Direct3D 9.2   ShaderModel 3
		D3D_FEATURE_LEVEL_9_1,	// Direct3D 9.1   ShaderModel 3
	};
	UINT numFeatureLevels = ARRAYSIZE(featureLevels);

	for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)//ハードウェア、WARP、リファレンスの順でデバイス作成を試みる
	{
		g_driverType = driverTypes[driverTypeIndex];
		hr = D3D11CreateDevice(nullptr, g_driverType, nullptr, createDeviceFlags, featureLevels, numFeatureLevels,
			D3D11_SDK_VERSION, &g_pd3dDevice, &g_featureLevel, &g_pImmediateContext);

		if (hr == E_INVALIDARG)
		{
			hr = D3D11CreateDevice(nullptr, g_driverType, nullptr, createDeviceFlags, &featureLevels[1], numFeatureLevels - 1,
				D3D11_SDK_VERSION, &g_pd3dDevice, &g_featureLevel, &g_pImmediateContext);
		}

		if (SUCCEEDED(hr))
			break;
	}
	if (FAILED(hr))
		return hr;

	//ファクトリーを作成する
	IDXGIFactory1* dxgiFactory = nullptr;
	{
		IDXGIDevice* dxgiDevice = nullptr;
		hr = g_pd3dDevice->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(&dxgiDevice));
		if (SUCCEEDED(hr))
		{
			IDXGIAdapter* adapter = nullptr;
			hr = dxgiDevice->GetAdapter(&adapter);
			if (SUCCEEDED(hr))
			{
				hr = adapter->GetParent(__uuidof(IDXGIFactory1), reinterpret_cast<void**>(&dxgiFactory));
				adapter->Release();
			}
			dxgiDevice->Release();
		}
	}
	if (FAILED(hr))
		return hr;

	IDXGIFactory2* dxgiFactory2 = nullptr;
	hr = dxgiFactory->QueryInterface(__uuidof(IDXGIFactory2), reinterpret_cast<void**>(&dxgiFactory2));
	if (dxgiFactory2)
	{
		hr = g_pd3dDevice->QueryInterface(__uuidof(ID3D11Device1), reinterpret_cast<void**>(&g_pd3dDevice1));
		if (SUCCEEDED(hr))
		{
			(void)g_pImmediateContext->QueryInterface(__uuidof(ID3D11DeviceContext1), reinterpret_cast<void**>(&g_pImmediateContext1));
		}

		DXGI_SWAP_CHAIN_DESC1 sd;
		ZeroMemory(&sd, sizeof(sd));
		sd.Width = width;
		sd.Height = height;
		sd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Quality = 0;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.BufferCount = 1;
		sd.SampleDesc.Quality = 0;
		sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
		//スワップチェインを作成する
		hr = dxgiFactory2->CreateSwapChainForHwnd(g_pd3dDevice, WINDOW::m_hWnd, &sd, nullptr, nullptr, &g_pSwapChain1);
		if (SUCCEEDED(hr))
		{
			hr = g_pSwapChain1->QueryInterface(__uuidof(IDXGISwapChain), reinterpret_cast<void**>(&g_pSwapChain));
		}

		dxgiFactory2->Release();
	}
	else
	{
		//スワップチェインの構造体
		DXGI_SWAP_CHAIN_DESC sd;
		ZeroMemory(&sd, sizeof(sd));
		sd.BufferCount = 1;
		sd.BufferDesc.Width = width;
		sd.BufferDesc.Height = height;
		sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		sd.BufferDesc.RefreshRate.Numerator = 60;
		sd.BufferDesc.RefreshRate.Denominator = 1;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.OutputWindow = WINDOW::m_hWnd;
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Quality = 0;
		sd.Windowed = TRUE;
		sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

		hr = dxgiFactory->CreateSwapChain(g_pd3dDevice, &sd, &g_pSwapChain);
	}

	//dxgiFactory->MakeWindowAssociation(WINDOW::m_hWnd, DXGI_MWA_VALID);

	dxgiFactory->Release();

	if (FAILED(hr))
		return hr;

	//=============================================================
	//描画機能に付随するほかのsingletonクラスを生成
	//=============================================================

	ShaderFactory::GetInstance().init(g_pd3dDevice, g_pImmediateContext);;//シェーダーファクトリーを生成する

	TextureFactory::GetInstance();//テクスチャファクトリーを生成する

	DirectWrite::GetInstance().Initialize(g_pSwapChain, pcd->hWnd);;//DirectWriteを生成する

	//========================================
	// その他の初期化事項 
	//========================================

	DIRECT3D11::InitBackBuffer();//バックバッファの初期化
	DIRECT3D11::InitBaseShader();//基本描画用シェーダの初期化
	DIRECT3D11::InitLayer();     //レイヤー機能の初期化
	DIRECT3D11::InitPostEffect();//ポストエフェクトの初期化


	Log("Direc3Dの初期化完了\n");

	return S_OK;
}


HRESULT DIRECT3D11::InitBackBuffer()
{
	HRESULT hr = S_OK;

	//バックバッファを作成
	ID3D11Texture2D* pBackBuffer = nullptr;
	hr = g_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&pBackBuffer));
	if (FAILED(hr))
		return hr;

	// バック・バッファの情報
	D3D11_TEXTURE2D_DESC descBackBuffer;
	pBackBuffer->GetDesc(&descBackBuffer);

	//レンダーターゲットビューを作成
	hr = g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_pBackBufferRTV);
	pBackBuffer->Release();
	if (FAILED(hr))
		return hr;

	// レンダーターゲットビューを作成
	ShaderUtility shaderUtility;
	shaderUtility.CreateRenderTargetView(g_pd3dDevice, &g_pInputRTV, viewWidth, viewHeight, DXGI_FORMAT_R16G16B16A16_FLOAT);

	//ブレンドステート初期化
	D3D11_BLEND_DESC bd;
	ZeroMemory(&bd, sizeof(D3D11_BLEND_DESC));
	bd.IndependentBlendEnable = false;
	bd.AlphaToCoverageEnable = false;
	bd.RenderTarget[0].BlendEnable = true;
	bd.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	bd.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
	bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	//ブレンドステート作成
	hr = g_pd3dDevice->CreateBlendState(&bd, &m_pBlendState);
	if (FAILED(hr))return hr;

	//深度ステンシルバッファ作成
	D3D11_TEXTURE2D_DESC txDesc = descBackBuffer;
	ZeroMemory(&txDesc, sizeof(txDesc));

	txDesc.Width = descBackBuffer.Width;
	txDesc.Height = descBackBuffer.Height;
	txDesc.MipLevels = 1;
	txDesc.ArraySize = 1;
	txDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	txDesc.SampleDesc.Count = 1;
	txDesc.SampleDesc.Quality = 0;
	txDesc.Usage = D3D11_USAGE_DEFAULT;
	txDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	txDesc.CPUAccessFlags = 0;
	txDesc.MiscFlags = 0;
	hr = g_pd3dDevice->CreateTexture2D(&txDesc, NULL, &m_pDepthStencilTexture);
	if (FAILED(hr))
		return hr;

	D3D11_DEPTH_STENCIL_VIEW_DESC dsDesc;
	ZeroMemory(&dsDesc, sizeof(dsDesc));
	dsDesc.Format = txDesc.Format;
	dsDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsDesc.Texture2D.MipSlice = 0;
	hr = g_pd3dDevice->CreateDepthStencilView(m_pDepthStencilTexture, &dsDesc, &m_pDepthStencilView);
	if (FAILED(hr))
		return hr;

	//ビューポートを設定
	vp[0].Width = (FLOAT)descBackBuffer.Width;
	vp[0].Height = (FLOAT)descBackBuffer.Height;
	vp[0].MinDepth = 0.0f;
	vp[0].MaxDepth = 1.0f;
	vp[0].TopLeftX = 0;
	vp[0].TopLeftY = 0;



	vp[1].Width = (FLOAT)descBackBuffer.Width / 2;
	vp[1].Height = (FLOAT)descBackBuffer.Height / 2;
	vp[1].MinDepth = 0.0f;
	vp[1].MaxDepth = 1.0f;
	vp[1].TopLeftX = (FLOAT)descBackBuffer.Width / 2;
	vp[1].TopLeftY = 0;

	g_pImmediateContext->RSSetViewports(1, &vp[0]);


	return S_OK;
}

HRESULT DIRECT3D11::InitBaseShader()
{
	HRESULT hr = E_FAIL;
	//シェーダークラスの生成と初期化
	m_pBaseShading = new BASE_SHADING();
	// BaseShadingの初期化
	hr = m_pBaseShading->Init(*g_pd3dDevice, *g_pImmediateContext);
	if (FAILED(hr))return hr;

	return S_OK;
}

bool DIRECT3D11::InitLayer()
{
	m_pDrawStorage = new DrawStorage();
	m_pDrawStorage->init(m_pBaseShading, g_pImmediateContext);
	return true;
}
bool DIRECT3D11::InitPostEffect()
{
	//----------------------------
	// ポストエフェクト管理クラス
	//----------------------------

	m_pPostEffectChain = new PostEffectChain();
	m_pPostEffectChain->init(g_pd3dDevice, g_pImmediateContext, viewWidth, viewHeight);

	//----------------------------
	// ポストエフェクト
	//----------------------------

	//色空間調整フィルター
	m_pHSVFilter = new HSVFilter();
	m_pHSVFilter->init(g_pd3dDevice, g_pImmediateContext, viewWidth, viewHeight);
	m_pPostEffectChain->addEffect(m_pHSVFilter);
	m_pHSVFilter->setIsApply(true);



	//水滴エフェクトフィルター
	m_pDropletFilter = new DropletFilter();
	m_pDropletFilter->init(g_pd3dDevice, g_pImmediateContext, (float)viewWidth / 2, (float)viewHeight / 2);
	m_pPostEffectChain->addEffect(m_pDropletFilter);
	m_pDropletFilter->setIsApply(false);

	//ガウス関数ぼかしフィルター
	m_pGaussianBlurFilter = new GaussianBlurFilter();
	m_pGaussianBlurFilter->init(g_pd3dDevice, g_pImmediateContext, viewWidth, viewHeight);
	m_pPostEffectChain->addEffect(m_pGaussianBlurFilter);
	m_pGaussianBlurFilter->setIsApply(false);

	//円形ぼかしフィルター
	m_pDiskBlurFilter = new DiskBlurFilter();
	m_pDiskBlurFilter->init(g_pd3dDevice, g_pImmediateContext, viewWidth, viewHeight);
	m_pPostEffectChain->addEffect(m_pDiskBlurFilter);
	m_pDiskBlurFilter->setIsApply(false);


	return true;
}


void DIRECT3D11::SetViewportId(int id)
{
	switch (id)
	{
	case 0:g_pImmediateContext->RSSetViewports(1, &vp[0]);
		break;
	case 1:g_pImmediateContext->RSSetViewports(1, &vp[1]);
		break;
	default:
		break;

	}
}

void DIRECT3D11::Clear()
{
	if (_isPostProcessing)//ポストエフェクトを適用するために入力RTVをクリアする
	{
		g_pImmediateContext->OMSetRenderTargets(1, &g_pInputRTV, nullptr);
		//画面クリア（実際は単色で画面を塗りつぶす処理）
		float ClearColor[4] = { 0,0,0,1 };// クリア色作成　RGBAの順
		g_pImmediateContext->ClearRenderTargetView(g_pInputRTV, ClearColor);//画面クリア
	}
	else
	{
		//適用しないならばバックバッファをセットする
		g_pImmediateContext->OMSetRenderTargets(1, &g_pBackBufferRTV, nullptr);
	}
	SetDefaultSampler();
	//画面クリア（実際は単色で画面を塗りつぶす処理）
	float ClearColor[4] = { 0,0,0,1 };// クリア色作成　RGBAの順
	g_pImmediateContext->ClearRenderTargetView(g_pBackBufferRTV, ClearColor);//画面クリア
	g_pImmediateContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);//深度バッファクリア
}
//
void DIRECT3D11::ApplyPostProcessing()
{
	if (_isPostProcessing)
	{
		m_pPostEffectChain->apply(g_pInputRTV, g_pBackBufferRTV);

	}

}
//
HRESULT DIRECT3D11::Present()
{
	g_pSwapChain->Present(1, 0);//画面更新（バックバッファをフロントバッファに）
	return S_OK;
}

//ガンマ設定
HRESULT DIRECT3D11::SetGamma(float gamma)
{
	// スワップ チェインのIDXGIOutputインターフェイスを取得
	IDXGIOutput* pOutput;
	HRESULT hr = g_pSwapChain->GetContainingOutput(&pOutput);
	if (FAILED(hr))return hr;

	// トーンカーブの設定を取得
	hr = pOutput->GetGammaControlCapabilities(&gammacap);
	if (FAILED(hr)) {
		pOutput->Release();
		return hr;	// 失敗
	}

	// トーンカーブを設定
	float g = 1.0f / gamma;
	gammacontrol.Scale.Red = 1.0f;
	gammacontrol.Scale.Green = 1.0f;
	gammacontrol.Scale.Blue = 1.0f;
	gammacontrol.Offset.Red = 0.0f;
	gammacontrol.Offset.Green = 0.0f;
	gammacontrol.Offset.Blue = 0.0f;
	for (UINT i = 0; i < gammacap.NumGammaControlPoints; ++i) {
		float L0 = gammacap.ControlPointPositions[i];
		float L1 = pow(L0, g);	// ガンマカーブを計算
		gammacontrol.GammaCurve[i].Red = L1;
		gammacontrol.GammaCurve[i].Green = L1;
		gammacontrol.GammaCurve[i].Blue = L1;
	}
	hr = pOutput->SetGammaControl(&gammacontrol);
	pOutput->Release();
	if (FAILED(hr))
		return hr;  // 失敗

	return hr;
}

void DIRECT3D11::IsSetDepthStencil(bool isApply)
{
	ID3D11RenderTargetView* old_RTV;
	g_pImmediateContext->OMGetRenderTargets(1, &old_RTV, nullptr);
	if (isApply)
	{
		g_pImmediateContext->OMSetRenderTargets(1, &old_RTV, m_pDepthStencilView);
	}
	else
	{
		g_pImmediateContext->OMSetRenderTargets(1, &old_RTV, nullptr);
	}
	SAFE_RELEASE(old_RTV);
}

HRESULT DIRECT3D11::SetFullScreen(bool isFullScreen)
{
	HRESULT hr = E_FAIL;
	hr = g_pSwapChain->SetFullscreenState(isFullScreen, NULL);
	if (FAILED(hr))return hr;
	return S_OK;
}

void DIRECT3D11::SetLayer(eLayer layer)
{
	elayer = layer;
}


void DIRECT3D11::ExecuteDraw()
{
	m_pDrawStorage->ExecuteDraw();
}

void DIRECT3D11::ExecuteDrawCameraUI()
{
	m_pDrawStorage->ExecuteDrawCameraUI();
}

HSVFilter* DIRECT3D11::getHSVFilter()const
{
	return m_pHSVFilter;
}

DiskBlurFilter* DIRECT3D11::getDiskBlurFilter()const
{
	return m_pDiskBlurFilter;
}

GaussianBlurFilter* DIRECT3D11::getGaussianBlurFilter()const
{
	return m_pGaussianBlurFilter;
}

DropletFilter* DIRECT3D11::getDropletFilter()const
{
	return m_pDropletFilter;
}


// ブレンド ステートを無効にするための設定を取得する
void DIRECT3D11::SetDefaultBlendDesc(int value)
{
	m_pBaseShading->SetAlpha(value);
	BLENDSTATE blendState;
	D3D11_RENDER_TARGET_BLEND_DESC blendDesc;
	blendDesc = blendState.GetDefaultBlendDesc();
	eblendState = BlendState_Default;
	blendState.SetBlendState(g_pd3dDevice, g_pImmediateContext, &blendDesc, 1, FALSE);
}
// 線形合成用ブレンド ステートのための設定を取得する
void DIRECT3D11::SetAlignmentBlendDesc(int value)
{
	m_pBaseShading->SetAlpha(value);
	BLENDSTATE blendState;
	D3D11_RENDER_TARGET_BLEND_DESC blendDesc;
	blendDesc = blendState.GetAlignmentBlendDesc();
	eblendState = BlendState_Alignment;
	blendState.SetBlendState(g_pd3dDevice, g_pImmediateContext, &blendDesc, 1, FALSE);
}
// 加算合成用ブレンド ステートのための設定を取得する
void DIRECT3D11::SetAddBlendDesc(int value)
{
	m_pBaseShading->SetAlpha(value);
	BLENDSTATE blendState;
	D3D11_RENDER_TARGET_BLEND_DESC blendDesc;
	blendDesc = blendState.GetAddBlendDesc();
	eblendState = BlendState_Add;
	blendState.SetBlendState(g_pd3dDevice, g_pImmediateContext, &blendDesc, 1, FALSE);
}
// 減算合成用ブレンド ステートのための設定を取得する
void DIRECT3D11::SetSubtractBlendDesc(int value)
{
	m_pBaseShading->SetAlpha(value);
	BLENDSTATE blendState;
	D3D11_RENDER_TARGET_BLEND_DESC blendDesc;
	blendDesc = blendState.GetSubtractBlendDesc();
	eblendState = BlendState_Subtract;
	blendState.SetBlendState(g_pd3dDevice, g_pImmediateContext, &blendDesc, 1, FALSE);
}
// 積算合成用ブレンド ステートのための設定を取得する
void DIRECT3D11::SetMultipleBlendDesc(int value)
{
	m_pBaseShading->SetAlpha(value);
	BLENDSTATE blendState;
	D3D11_RENDER_TARGET_BLEND_DESC blendDesc;
	blendDesc = blendState.GetMultipleBlendDesc();
	eblendState = BlendState_Multiple;
	blendState.SetBlendState(g_pd3dDevice, g_pImmediateContext, &blendDesc, 1, FALSE);
}

// ブレンドステートを設定する
void DIRECT3D11::SetBlendDesc(eBlendState blendstate, int value)
{
	switch (blendstate)
	{
	case BlendState_Default:
		SetDefaultBlendDesc(value);
		break;
	case BlendState_Alignment:
		SetAlignmentBlendDesc(value);
		break;
	case BlendState_Add:
		SetAddBlendDesc(value);
		break;
	case BlendState_Subtract:
		SetSubtractBlendDesc(value);
		break;
	case BlendState_Multiple:
		SetMultipleBlendDesc(value);
		break;
	default:
		break;
	}
}

void DIRECT3D11::SetDefaultSampler()
{
	SAMPLERSTATE samplerState;
	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc = samplerState.GetDefaultSamplerDesc();
	samplerState.SetSamplerState(g_pd3dDevice, g_pImmediateContext, samplerDesc);
}

void DIRECT3D11::SetFocusHoleSampler()
{
	SAMPLERSTATE samplerState;
	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc = samplerState.GetFocusHoleSamplerDesc();
	samplerState.SetSamplerState(g_pd3dDevice, g_pImmediateContext, samplerDesc);
}

void DIRECT3D11::DrawPoint(float x, float y, float* color)//点を描画する
{

	Vertex2D v[1]//頂点座標
	{
		{{x , y , 0},{color[0],color[1],color[2],color[3]}, {0, 1}},
	};
	if (_isApplyLayer == true && elayer != Layer_None)
	{
		Command_2D instanced;
		instanced.layer = elayer;
		instanced.drawCommand = DrawComamnd_Point;
		for (int i = 0; i < ARRAYSIZE(v); i++)
		{
			instanced.v.emplace_back(v[i]);
		}
		instanced.vertexNum = ARRAYSIZE(v);
		instanced.blendState = eblendState;
		instanced.alpha = m_pBaseShading->GetAlpha();
		instanced.texId = -1;
		instanced.m_pDepthStencilState = nullptr;
		m_pDrawStorage->setDrawCommand2D(&instanced);
		return;
	}
	//-----------------------------  
	// シェーダーをセット
	//----------------------------
	m_pBaseShading->SetShader(*g_pImmediateContext, VERTEX_SHADER_World, PIXEL_SHADER_2D_Raw);
	m_pBaseShading->SetInputLayout(*g_pImmediateContext, LAYOUT_2D);
	//頂点情報をバッファに書き込む
	m_pBaseShading->WriteVertexInfo2D(*g_pImmediateContext, *v, sizeof(v));

	// プロミティブ・トポロジーをセット
	g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);



	g_pImmediateContext->Draw(1, 0);
}

void DIRECT3D11::DrawLine(float x1, float y1, float x2, float y2, float* color)//線を描画する
{
	Vertex2D v[2]//二点間の頂点座標
	{
		{{x1 , y1 , 0},{color[0],color[1],color[2],color[3]}, {0, 1}},
		{{x2 , y2 , 0},{color[0],color[1],color[2],color[3]}, {0, 0}},
	};
	if (_isApplyLayer == true && elayer != Layer_None)
	{
		Command_2D instanced;
		instanced.layer = elayer;
		instanced.drawCommand = DrawComamnd_Line;
		for (int i = 0; i < ARRAYSIZE(v); i++)
		{
			instanced.v.emplace_back(v[i]);
		}
		instanced.vertexNum = ARRAYSIZE(v);
		instanced.blendState = eblendState;
		instanced.alpha = m_pBaseShading->GetAlpha();
		instanced.texId = -1;
		instanced.m_pDepthStencilState = nullptr;
		m_pDrawStorage->setDrawCommand2D(&instanced);
		return;
	}
	//-----------------------------  
	// シェーダーをセット
	//----------------------------
	m_pBaseShading->SetShader(*g_pImmediateContext, VERTEX_SHADER_World, PIXEL_SHADER_2D_Raw);
	m_pBaseShading->SetInputLayout(*g_pImmediateContext, LAYOUT_2D);
	//頂点情報をバッファに書き込む
	m_pBaseShading->WriteVertexInfo2D(*g_pImmediateContext, *v, sizeof(v));

	// プロミティブ・トポロジーをセット
	g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	g_pImmediateContext->Draw(2, 0);
}


void DIRECT3D11::DrawBox(float x, float y, float w, float h, float* color)//クアッドを描画する
{

	Vertex2D v[4] = {
			{{x , y + h , 0},{color[0],color[1],color[2],color[3]}, {0, 1}},	// 左下
			{{x , y , 0}, {color[0],color[1],color[2],color[3]},{0, 0}},	// 左上
			{{x + w , y + h , 0}, {color[0],color[1],color[2],color[3]},{1, 1}},	// 右下
			{{x + w , y , 0}, {color[0],color[1],color[2],color[3]},{1, 0}},	// 右上
	};
	if (_isApplyLayer == true && elayer != Layer_None)
	{
		Command_2D instanced;
		instanced.layer = elayer;
		instanced.drawCommand = DrawComamnd_Rect_Color;
		for (int i = 0; i < ARRAYSIZE(v); i++)
		{
			instanced.v.emplace_back(v[i]);
		}
		instanced.vertexNum = ARRAYSIZE(v);
		instanced.blendState = eblendState;
		instanced.alpha = m_pBaseShading->GetAlpha();
		instanced.texId = -1;
		instanced.m_pDepthStencilState = nullptr;
		m_pDrawStorage->setDrawCommand2D(&instanced);
		return;
	}

	//-----------------------------  
	// シェーダーをセット
	//----------------------------
	m_pBaseShading->SetShader(*g_pImmediateContext, VERTEX_SHADER_World, PIXEL_SHADER_2D_Raw);
	m_pBaseShading->SetInputLayout(*g_pImmediateContext, LAYOUT_2D);
	//頂点情報をバッファに書き込む
	m_pBaseShading->WriteVertexInfo2D(*g_pImmediateContext, *v, sizeof(v));

	// プロミティブ・トポロジーをセット
	g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	g_pImmediateContext->Draw(4, 0);
}


void DIRECT3D11::DrawRotBox(float x, float y, float w, float h, float angle, float size, float* color)//回転するクアッドを描画する
{

	Vertex2D v[4] = {
			{{-(w / 2) , (h / 2) , 0},{color[0],color[1],color[2],color[3]}, {0, 1}},	// 左下
			{{-(w / 2) , -(h / 2) , 0}, {color[0],color[1],color[2],color[3]},{0, 0}},	// 左上
			{{(w / 2) , (h / 2) , 0}, {color[0],color[1],color[2],color[3]},{1, 1}},	// 右下
			{{(w / 2) , -(h / 2) , 0}, {color[0],color[1],color[2],color[3]},{1, 0}},	// 右上
	};
	float POSX[4];
	float POSY[4];
	//角度を行列に代入
	for (int i = 0; i < 4; i++)
	{
		POSX[i] = (v[i].Pos.x * size) * cos(angle) - (v[i].Pos.y * size) * sin(angle);
		POSY[i] = (v[i].Pos.x * size) * sin(angle) + (v[i].Pos.y * size) * cos(angle);

		v[i].Pos.x = POSX[i] + x;
		v[i].Pos.y = POSY[i] + y;
	}
	if (_isApplyLayer == true && elayer != Layer_None)
	{
		Command_2D instanced;
		instanced.layer = elayer;
		instanced.drawCommand = DrawComamnd_Rect_Color;
		for (int i = 0; i < ARRAYSIZE(v); i++)
		{
			instanced.v.emplace_back(v[i]);
		}
		instanced.vertexNum = ARRAYSIZE(v);
		instanced.blendState = eblendState;
		instanced.alpha = m_pBaseShading->GetAlpha();
		instanced.texId = -1;
		instanced.m_pDepthStencilState = nullptr;
		m_pDrawStorage->setDrawCommand2D(&instanced);
		return;
	}
	//-----------------------------  
	// シェーダーをセット
	//----------------------------
	m_pBaseShading->SetShader(*g_pImmediateContext, VERTEX_SHADER_World, PIXEL_SHADER_2D_Raw);
	m_pBaseShading->SetInputLayout(*g_pImmediateContext, LAYOUT_2D);
	//頂点情報をバッファに書き込む
	m_pBaseShading->WriteVertexInfo2D(*g_pImmediateContext, *v, sizeof(v));

	// プロミティブ・トポロジーをセット
	g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	g_pImmediateContext->Draw(4, 0);
}

void DIRECT3D11::DrawImagebyName(std::string texName, float x, float y, float w, float h)
{
	int texId = TEX_FAC.getIDbyName(texName);
	auto tex = TEX_FAC.getTexturebyId(texId);
	if (!tex)
	{
		DrawBox(x, y, w, h, errorColor);
		return;
	}

	// 頂点データ作成
	Vertex2D v[4] = {
			{{x , y + h , 0},{0,0,0,0}, {0, 1}},	// 左下
			{{x , y , 0},{0,0,0,0}, {0, 0}},	// 左上
			{{x + w , y + h , 0},{0,0,0,0}, {1, 1}},	// 右下
			{{x + w , y , 0},{0,0,0,0}, {1, 0}},	// 右上
	};
	if (_isApplyLayer == true && elayer != Layer_None)
	{
		Command_2D instanced;
		instanced.layer = elayer;
		instanced.drawCommand = DrawComamnd_Rect_Texture;
		for (int i = 0; i < ARRAYSIZE(v); i++)
		{
			instanced.v.emplace_back(v[i]);
		}
		instanced.vertexNum = ARRAYSIZE(v);
		instanced.blendState = eblendState;
		instanced.alpha = m_pBaseShading->GetAlpha();
		instanced.texId = texId;
		instanced.m_pDepthStencilState = nullptr;
		m_pDrawStorage->setDrawCommand2D(&instanced);
		return;
	}
	//-----------------------------
	// シェーダーをセット
	//----------------------------- 
	m_pBaseShading->SetShader(*g_pImmediateContext, VERTEX_SHADER_World, PIXEL_SHADER_2D_Texture);
	m_pBaseShading->SetInputLayout(*g_pImmediateContext, LAYOUT_2D);

	//頂点情報をバッファに書き込む
	m_pBaseShading->WriteVertexInfo2D(*g_pImmediateContext, *v, sizeof(v));

	// テクスチャを、スロット0にセット
	g_pImmediateContext->PSSetShaderResources(0, 1, tex->getTexResource());

	// プロミティブ・トポロジーをセット
	g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	g_pImmediateContext->Draw(4, 0);
}

void DIRECT3D11::DrawImage(int texId, float x, float y, float w, float h)//テクスチャ付きのクアッドを描画する
{
	auto tex = TEX_FAC.getTexturebyId(texId);

	if (!tex)
	{
		DrawBox(x, y, w, h, errorColor);
		return;
	}

	// 頂点データ作成
	Vertex2D v[4] = {
			{{x , y + h , 0},{0,0,0,0}, {0, 1}},	// 左下
			{{x , y , 0},{0,0,0,0}, {0, 0}},	// 左上
			{{x + w , y + h , 0},{0,0,0,0}, {1, 1}},	// 右下
			{{x + w , y , 0},{0,0,0,0}, {1, 0}},	// 右上
	};
	if (_isApplyLayer == true && elayer != Layer_None)
	{
		Command_2D instanced;
		instanced.layer = elayer;
		instanced.drawCommand = DrawComamnd_Rect_Texture;
		for (int i = 0; i < ARRAYSIZE(v); i++)
		{
			instanced.v.emplace_back(v[i]);
		}
		instanced.vertexNum = ARRAYSIZE(v);
		instanced.blendState = eblendState;
		instanced.alpha = m_pBaseShading->GetAlpha();
		instanced.texId = texId;
		instanced.m_pDepthStencilState = nullptr;
		m_pDrawStorage->setDrawCommand2D(&instanced);
		return;
	}
	//-----------------------------
	// シェーダーをセット
	//----------------------------- 
	m_pBaseShading->SetShader(*g_pImmediateContext, VERTEX_SHADER_World, PIXEL_SHADER_2D_Texture);
	m_pBaseShading->SetInputLayout(*g_pImmediateContext, LAYOUT_2D);

	//頂点情報をバッファに書き込む
	m_pBaseShading->WriteVertexInfo2D(*g_pImmediateContext, *v, sizeof(v));

	// テクスチャを、スロット0にセット
	g_pImmediateContext->PSSetShaderResources(0, 1, tex->getTexResource());

	// プロミティブ・トポロジーをセット
	g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	g_pImmediateContext->Draw(4, 0);
}

void DIRECT3D11::DrawImageFlip(int texId, float x, float y, float w, float h, bool isFlip)//テクスチャ付きのクアッドを描画する
{
	auto tex = TEX_FAC.getTexturebyId(texId);
	if (!tex)
	{
		DrawBox(x, y, w, h, errorColor);
		return;
	}
	// 頂点データ作成
	Vertex2D v[4] = {
			{{x , y + h , 0},    {0,0,0,0}, {(isFlip ? 0.0f : 1.0f), 1.0f}},	// 左下
			{{x , y , 0},        {0,0,0,0}, {(isFlip ? 0.0f : 1.0f), 0.0f}},	// 左上
			{{x + w , y + h , 0},{0,0,0,0}, {(isFlip ? 1.0f : 0.0f), 1.0f}},	// 右下
			{{x + w , y , 0},    {0,0,0,0}, {(isFlip ? 1.0f : 0.0f), 0.0f}},	// 右上
	};
	if (_isApplyLayer == true && elayer != Layer_None)
	{
		Command_2D instanced;
		instanced.layer = elayer;
		instanced.drawCommand = DrawComamnd_Rect_Texture;
		for (int i = 0; i < ARRAYSIZE(v); i++)
		{
			instanced.v.emplace_back(v[i]);
		}
		instanced.vertexNum = ARRAYSIZE(v);
		instanced.blendState = eblendState;
		instanced.alpha = m_pBaseShading->GetAlpha();
		instanced.texId = texId;
		instanced.m_pDepthStencilState = nullptr;
		m_pDrawStorage->setDrawCommand2D(&instanced);
		return;
	}
	//-----------------------------
	// シェーダーをセット
	//----------------------------- 
	m_pBaseShading->SetShader(*g_pImmediateContext, VERTEX_SHADER_World, PIXEL_SHADER_2D_Texture);
	m_pBaseShading->SetInputLayout(*g_pImmediateContext, LAYOUT_2D);

	//頂点情報をバッファに書き込む
	m_pBaseShading->WriteVertexInfo2D(*g_pImmediateContext, *v, sizeof(v));

	// テクスチャを、スロット0にセット
	g_pImmediateContext->PSSetShaderResources(0, 1, tex->getTexResource());

	// プロミティブ・トポロジーをセット
	g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	g_pImmediateContext->Draw(4, 0);
}

void DIRECT3D11::DrawRotImage(int texId, float x, float y, float w, float h, float angle, float size)//テクスチャ付きの回転するクアッドを描画する
{
	auto tex = TEX_FAC.getTexturebyId(texId);
	if (!tex)
	{
		DrawRotBox(x, y, w, h, angle, size, errorColor);
		return;
	}
	Vertex2D v[4] = {
			{{-(w / 2) , (h / 2) , 0},{0,0,0,0}, {0, 1}},	// 左下
			{{-(w / 2) , -(h / 2) , 0}, {0,0,0,0},{0, 0}},	// 左上
			{{(w / 2) , (h / 2) , 0}, {0,0,0,0},{1, 1}},	// 右下
			{{(w / 2) , -(h / 2) , 0}, {0,0,0,0},{1, 0}},	// 右上
	};


	float POSX[4];
	float POSY[4];
	//角度を行列に代入
	for (int i = 0; i < 4; i++)
	{
		POSX[i] = (v[i].Pos.x * size) * cos(angle) - (v[i].Pos.y * size) * sin(angle);
		POSY[i] = (v[i].Pos.x * size) * sin(angle) + (v[i].Pos.y * size) * cos(angle);

		v[i].Pos.x = POSX[i] + x;
		v[i].Pos.y = POSY[i] + y;
	}
	if (_isApplyLayer == true && elayer != Layer_None)
	{
		Command_2D instanced;
		instanced.layer = elayer;
		instanced.drawCommand = DrawComamnd_Rect_Texture;
		for (int i = 0; i < ARRAYSIZE(v); i++)
		{
			instanced.v.emplace_back(v[i]);
		}
		instanced.vertexNum = ARRAYSIZE(v);
		instanced.blendState = eblendState;
		instanced.alpha = m_pBaseShading->GetAlpha();
		instanced.texId = texId;
		instanced.m_pDepthStencilState = nullptr;
		m_pDrawStorage->setDrawCommand2D(&instanced);
		return;
	}
	//----------------------------
	// シェーダーをセット
	//----------------------------
	m_pBaseShading->SetShader(*g_pImmediateContext, VERTEX_SHADER_World, PIXEL_SHADER_2D_Texture);
	m_pBaseShading->SetInputLayout(*g_pImmediateContext, LAYOUT_2D);
	//頂点情報をバッファに書き込む
	m_pBaseShading->WriteVertexInfo2D(*g_pImmediateContext, *v, sizeof(v));
	// テクスチャを、スロット0にセット
	g_pImmediateContext->PSSetShaderResources(0, 1, tex->getTexResource());
	// プロミティブ・トポロジーをセット
	g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	g_pImmediateContext->Draw(4, 0);
}

void DIRECT3D11::DrawRotFlipImage(int texId, float x, float y, float w, float h, float angle, float size,bool isFlip)//テクスチャ付きの回転するクアッドを描画する
{
	auto tex = TEX_FAC.getTexturebyId(texId);
	if (!tex)
	{
		DrawRotBox(x, y, w, h, angle, size, errorColor);
		return;
	}
	Vertex2D v[4] = {
			{{-(w / 2) , (h / 2) , 0},  {0,0,0,0},{isFlip ? 0.0f : 1.0f, 1}},	// 左下
			{{-(w / 2) , -(h / 2) , 0}, {0,0,0,0},{isFlip ? 0.0f : 1.0f, 0}},	// 左上
			{{(w / 2) , (h / 2) , 0},   {0,0,0,0},{isFlip ? 1.0f : 0.0f, 1}},	// 右下
			{{(w / 2) , -(h / 2) , 0},  {0,0,0,0},{isFlip ? 1.0f : 0.0f, 0}},	// 右上
	};


	float POSX[4];
	float POSY[4];
	//角度を行列に代入
	for (int i = 0; i < 4; i++)
	{
		POSX[i] = (v[i].Pos.x * size) * cos(angle) - (v[i].Pos.y * size) * sin(angle);
		POSY[i] = (v[i].Pos.x * size) * sin(angle) + (v[i].Pos.y * size) * cos(angle);

		v[i].Pos.x = POSX[i] + x;
		v[i].Pos.y = POSY[i] + y;
	}
	if (_isApplyLayer == true && elayer != Layer_None)
	{
		Command_2D instanced;
		instanced.layer = elayer;
		instanced.drawCommand = DrawComamnd_Rect_Texture;
		for (int i = 0; i < ARRAYSIZE(v); i++)
		{
			instanced.v.emplace_back(v[i]);
		}
		instanced.vertexNum = ARRAYSIZE(v);
		instanced.blendState = eblendState;
		instanced.alpha = m_pBaseShading->GetAlpha();
		instanced.texId = texId;
		instanced.m_pDepthStencilState = nullptr;
		m_pDrawStorage->setDrawCommand2D(&instanced);
		return;
	}
	//----------------------------
	// シェーダーをセット
	//----------------------------
	m_pBaseShading->SetShader(*g_pImmediateContext, VERTEX_SHADER_World, PIXEL_SHADER_2D_Texture);
	m_pBaseShading->SetInputLayout(*g_pImmediateContext, LAYOUT_2D);
	//頂点情報をバッファに書き込む
	m_pBaseShading->WriteVertexInfo2D(*g_pImmediateContext, *v, sizeof(v));
	// テクスチャを、スロット0にセット
	g_pImmediateContext->PSSetShaderResources(0, 1, tex->getTexResource());
	// プロミティブ・トポロジーをセット
	g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	g_pImmediateContext->Draw(4, 0);
}

void DIRECT3D11::DrawExtendedImage(int texId, float x, float y, float w, float h, float xSize, float ySize)
{
	auto tex = TEX_FAC.getTexturebyId(texId);
	if (!tex)
	{
		DrawBox(x, y, w, h, errorColor);
		return;
	}

	Vertex2D v[4] = {
			{{x - (w / 2) * xSize , y + (h / 2) * ySize , 0},{0,0,0,0}, {0, 1}},	// 左下
			{{x - (w / 2) * xSize , y - (w / 2) * ySize , 0}, {0,0,0,0},{0, 0}},	// 左上
			{{x + (w / 2) * xSize , y + (h / 2) * ySize , 0}, {0,0,0,0},{1, 1}},	// 右下
			{{x + (w / 2) * xSize , y - (h / 2) * ySize , 0}, {0,0,0,0},{1, 0}},	// 右上
	};

	if (_isApplyLayer == true && elayer != Layer_None)
	{
		Command_2D instanced;
		instanced.layer = elayer;
		instanced.drawCommand = DrawComamnd_Rect_Texture;
		for (int i = 0; i < ARRAYSIZE(v); i++)
		{
			instanced.v.emplace_back(v[i]);
		}
		instanced.vertexNum = ARRAYSIZE(v);
		instanced.blendState = eblendState;
		instanced.alpha = m_pBaseShading->GetAlpha();
		instanced.texId = texId;
		instanced.m_pDepthStencilState = nullptr;
		m_pDrawStorage->setDrawCommand2D(&instanced);
		return;
	}
	m_pBaseShading->SetShader(*g_pImmediateContext, VERTEX_SHADER_World, PIXEL_SHADER_2D_Texture);
	m_pBaseShading->SetInputLayout(*g_pImmediateContext, LAYOUT_2D);
	//頂点情報をバッファに書き込む
	m_pBaseShading->WriteVertexInfo2D(*g_pImmediateContext, *v, sizeof(v));
	// テクスチャを、スロット0にセット
	g_pImmediateContext->PSSetShaderResources(0, 1, tex->getTexResource());
	// プロミティブ・トポロジーをセット
	g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	g_pImmediateContext->Draw(4, 0);
	//-----------------------------  
	// シェーダーをセット
	//----------------------------
}

void DIRECT3D11::DrawDivImage(int texId, float x, float y, float w, float h, float xi, float yi, int xSize, int ySize)//分割テクスチャ付きのクアッドを描画する
{
	auto tex = TEX_FAC.getTexturebyId(texId);
	if (!tex)
	{
		DrawBox(x, y, w, h, errorColor);
		return;
	}
	size_t imageXSize = tex->getTexInfo().width;
	size_t imageYSize = tex->getTexInfo().height;
	float left = 0, top = 0, right = 0, bottom = 0;
	left = xi;
	top = yi;
	right = left + xSize;
	bottom = top + ySize;
	left /= (float)imageXSize;
	top /= (float)imageYSize;
	right /= (float)imageXSize;
	bottom /= (float)imageYSize;
	Vertex2D v[4] =
	{
		//内側に織り込んでちらつきを防止しています
		{{x,y + h,0},{0,0,0,0}, {left + 0.0001f,bottom - 0.0001f}},	// 左下
		{{x , y , 0},{0,0,0,0}, {left + 0.0001f,top + 0.0001f}},	// 左上
		{{x + w , y + h,0},{0,0,0,0}, {right - 0.0001f,bottom - 0.0001f}},	// 右下
		{{x + w , y,0},{0,0,0,0}, {right - 0.0001f,top + 0.0001f}},	// 右上

	};

	if (_isApplyLayer == true && elayer != Layer_None)
	{
		Command_2D instanced;
		instanced.layer = elayer;
		instanced.drawCommand = DrawComamnd_Rect_Texture;
		for (int i = 0; i < ARRAYSIZE(v); i++)
		{
			instanced.v.emplace_back(v[i]);
		}
		instanced.vertexNum = ARRAYSIZE(v);
		instanced.blendState = eblendState;
		instanced.alpha = m_pBaseShading->GetAlpha();
		instanced.texId = texId;
		instanced.m_pDepthStencilState = nullptr;
		m_pDrawStorage->setDrawCommand2D(&instanced);
		return;
	}

	m_pBaseShading->SetShader(*g_pImmediateContext, VERTEX_SHADER_World, PIXEL_SHADER_2D_Texture);
	m_pBaseShading->SetInputLayout(*g_pImmediateContext, LAYOUT_2D);
	//頂点情報をバッファに書き込む
	m_pBaseShading->WriteVertexInfo2D(*g_pImmediateContext, *v, sizeof(v));
	// テクスチャを、スロット0にセット
	g_pImmediateContext->PSSetShaderResources(0, 1, tex->getTexResource());
	// プロミティブ・トポロジーをセット
	g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	g_pImmediateContext->Draw(4, 0);
}

void DIRECT3D11::DrawInstancedImage(int texId, float x, float y,
	const InstanceData2D& data, size_t dataSize, int mapSizeX, int mapSizeY) //テクスチャ付きクアッドをインスタンス描画する。
{
	auto tex = TEX_FAC.getTexturebyId(texId);
	if (!tex)return;

	//この頂点データを原本としてGPU側で指定された分だけ描画する
	InstanceData2D v[] = {
	{{0, (float)Define::ChipSize , 0}, {0, (float)1 / mapSizeY}},	// 左下
	{{0 , 0 , 0}, {0, 0}},	// 左上
	{{(float)Define::ChipSize , (float)Define::ChipSize , 0}, {(float)1 / mapSizeX, (float)1 / mapSizeY}},	// 右下
	{{(float)Define::ChipSize , 0 , 0},{(float)1 / mapSizeX, 0}},	// 右上
	};

	if (_isApplyLayer == true && elayer != Layer_None)
	{
		Command_2D_Instanced instanced;
		instanced.layer = elayer;
		instanced.drawCommand = DrawComamnd_Rect_Texture_Instanced;
		for (int i = 0; i < ARRAYSIZE(v); i++)
		{
			instanced.v.emplace_back(v[i]);
		}
		instanced.vertexNum = ARRAYSIZE(v);
		instanced.instancedData = &data;
		instanced.instanceNum = dataSize;
		instanced.blendState = eblendState;
		instanced.alpha = m_pBaseShading->GetAlpha();
		instanced.texId = texId;
		instanced.m_pDepthStencilState = nullptr;
		m_pDrawStorage->setDrawCommand2D_Instanced(&instanced);

		return;
	}

	m_pBaseShading->WriteVertexInfoInstancing2D(*g_pImmediateContext, *v, sizeof(v), data, dataSize);
	// テクスチャを、スロット0にセット
	g_pImmediateContext->PSSetShaderResources(0, 1, tex->getTexResource());
	g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	g_pImmediateContext->DrawInstanced(4, dataSize, 0, 0);
}

void DIRECT3D11::DrawSRVTex(ID3D11ShaderResourceView* const* srv, float x, float y, float w, float h)//テクスチャ付きのクアッドを描画する
{

	if (srv == nullptr)
	{
		DrawBox(x, y, w, h, errorColor);
		return;
	}

	// 頂点データ作成
	Vertex2D v[4] = {
			{{x , y + h , 0},{0,0,0,0}, {0, 1}},	// 左下
			{{x , y , 0},{0,0,0,0}, {0, 0}},	// 左上
			{{x + w , y + h , 0},{0,0,0,0}, {1, 1}},	// 右下
			{{x + w , y , 0},{0,0,0,0}, {1, 0}},	// 右上
	};
	if (_isApplyLayer == true && elayer != Layer_None)
	{
		Command_2D instanced;
		instanced.layer = elayer;
		instanced.drawCommand = DrawComamnd_Rect_SRV;
		for (int i = 0; i < ARRAYSIZE(v); i++)
		{
			instanced.v.emplace_back(v[i]);
		}
		instanced.vertexNum = ARRAYSIZE(v);
		instanced.blendState = eblendState;
		instanced.alpha = m_pBaseShading->GetAlpha();
		instanced.texId = -1;
		instanced.m_pSRV = srv;
		instanced.m_pDepthStencilState = nullptr;
		m_pDrawStorage->setDrawCommand2D(&instanced);
		return;
	}
	//-----------------------------
	// シェーダーをセット
	//----------------------------- 
	m_pBaseShading->SetShader(*g_pImmediateContext, VERTEX_SHADER_World, PIXEL_SHADER_2D_Texture);
	m_pBaseShading->SetInputLayout(*g_pImmediateContext, LAYOUT_2D);

	//頂点情報をバッファに書き込む
	m_pBaseShading->WriteVertexInfo2D(*g_pImmediateContext, *v, sizeof(v));

	// テクスチャを、スロット0にセット
	g_pImmediateContext->PSSetShaderResources(0, 1, srv);

	// プロミティブ・トポロジーをセット
	g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	g_pImmediateContext->Draw(4, 0);
}

void DIRECT3D11::DrawCube(float posX, float posY, float posZ,
	float rotX, float rotY, float rotZ,
	float sclX, float sclY, float sclZ)
{
	// バーテックスバッファの作成
	Vertex3D v[] =
	{
	{ { -1.0f,  1.0f, -1.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } , {  0.0f,  0.0f, -1.0f }},
	{ {  1.0f,  1.0f, -1.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } , {  0.0f,  0.0f, -1.0f }},
	{ { -1.0f, -1.0f, -1.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } , {  0.0f,  0.0f, -1.0f }},
	{ {  1.0f, -1.0f, -1.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } , {  0.0f,  0.0f, -1.0f }},

	{ { -1.0f,  1.0f,  1.0f }, { 0.0f, 1.0f, 1.0f, 1.0f }, {  0.0f,  0.0f,  1.0f } },
	{ { -1.0f, -1.0f,  1.0f }, { 0.0f, 1.0f, 1.0f, 1.0f } ,{  0.0f,  0.0f,  1.0f }  },
	{ {  1.0f,  1.0f,  1.0f }, { 0.0f, 1.0f, 1.0f, 1.0f }, {  0.0f,  0.0f,  1.0f } },
	{ {  1.0f, -1.0f,  1.0f }, { 0.0f, 1.0f, 1.0f, 1.0f }, {  0.0f,  0.0f,  1.0f } },

	{ { -1.0f,  1.0f,  1.0f }, { 1.0f, 1.0f, 0.0f, 1.0f } , { -1.0f,  0.0f,  0.0f }},
	{ { -1.0f,  1.0f, -1.0f }, { 1.0f, 1.0f, 0.0f, 1.0f }, { -1.0f,  0.0f,  0.0f } },
	{ { -1.0f, -1.0f,  1.0f }, { 1.0f, 1.0f, 0.0f, 1.0f }, { -1.0f,  0.0f,  0.0f } },
	{ { -1.0f, -1.0f, -1.0f }, { 1.0f, 1.0f, 0.0f, 1.0f } , { -1.0f,  0.0f,  0.0f }},

	{ {  1.0f,  1.0f,  1.0f }, { 0.0f, 0.0f, 1.0f, 1.0f }, {  1.0f,  0.0f,  0.0f } },
	{ {  1.0f, -1.0f,  1.0f }, { 0.0f, 0.0f, 1.0f, 1.0f }, {  1.0f,  0.0f,  0.0f } },
	{ {  1.0f,  1.0f, -1.0f }, { 0.0f, 0.0f, 1.0f, 1.0f }, {  1.0f,  0.0f,  0.0f } },
	{ {  1.0f, -1.0f, -1.0f }, { 0.0f, 0.0f, 1.0f, 1.0f }, {  1.0f,  0.0f,  0.0f } },

	{ { -1.0f,  1.0f,  1.0f }, { 1.0f, 0.0f, 1.0f, 1.0f }, {  0.0f,  1.0f,  0.0f } },
	{ {  1.0f,  1.0f,  1.0f }, { 1.0f, 0.0f, 1.0f, 1.0f }, {  0.0f,  1.0f,  0.0f } },
	{ { -1.0f,  1.0f, -1.0f }, { 1.0f, 0.0f, 1.0f, 1.0f }, {  0.0f,  1.0f,  0.0f } },
	{ {  1.0f,  1.0f, -1.0f }, { 1.0f, 0.0f, 1.0f, 1.0f }, {  0.0f,  1.0f,  0.0f } },

	{ { -1.0f, -1.0f,  1.0f }, { 0.0f, 1.0f, 0.0f, 1.0f }, {  0.0f, -1.0f,  0.0f } },
	{ { -1.0f, -1.0f, -1.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } , {  0.0f, -1.0f,  0.0f }},
	{ {  1.0f, -1.0f,  1.0f }, { 0.0f, 1.0f, 0.0f, 1.0f }, {  0.0f, -1.0f,  0.0f } },
	{ {  1.0f, -1.0f, -1.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } , {  0.0f, -1.0f,  0.0f }},
	};

	if (_isApplyLayer == true && elayer != Layer_None)
	{
		
		Command_3D instanced;
		instanced.layer = elayer;
		instanced.drawCommand = DrawCommand_Cube;
		for (int i = 0; i < ARRAYSIZE(v); i++)
		{
			instanced.v.emplace_back(v[i]);
		}
		instanced.blendState = eblendState;
		instanced.alpha = m_pBaseShading->GetAlpha();

		m_pDrawStorage->setDrawCommand3D(&instanced);
		return;
	}
	//-----------------------------  
	// シェーダーをセット
	//----------------------------
	m_pBaseShading->SetShader(*g_pImmediateContext, VERTEX_SHADER_3D, PIXEL_SHADER_3D_Color);
	m_pBaseShading->SetInputLayout(*g_pImmediateContext, LAYOUT_3D);
	//頂点情報をバッファに書き込む
	m_pBaseShading->WriteVertexInfo3D(*g_pImmediateContext, *v, sizeof(v));

	// プリミティブトポロジの設定
	g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	g_pImmediateContext->DrawIndexed(36, 0, 0);
}

float* DIRECT3D11::GetColor(int r, int g, int b)
{
	float R = (float)r / 255;
	float G = (float)g / 255;
	float B = (float)b / 255;
	color[0] = R;
	color[1] = G;
	color[2] = B;
	color[3] = 1.0f;

	return color;
}

