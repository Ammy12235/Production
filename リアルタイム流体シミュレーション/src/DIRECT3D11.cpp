#include "DIRECT3D11.h"
#include "BaseShading.h"
#include "DEFINE.h"


//DirectXを初期化する
HRESULT DIRECT3D11::Init(D3D_INIT* pcd)
{

	HRESULT hr = S_OK;

	Log("Direc3Dの初期化を開始\n");
	RECT rc;
	GetClientRect(WINDOW::m_hWnd, &rc);//画面の幅、高さを取得、格納する
	UINT width = rc.right - rc.left;
	UINT height = rc.bottom - rc.top;

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

		hr = dxgiFactory->CreateSwapChain(g_pd3dDevice, &sd, &g_pSwapChain);
	}

	dxgiFactory->MakeWindowAssociation(WINDOW::m_hWnd, DXGI_MWA_NO_ALT_ENTER);

	dxgiFactory->Release();

	if (FAILED(hr))
		return hr;
	//=============================================================
	//描画機能に付随するほかのsingletonクラスを生成
	//=============================================================

	ShaderFactory::CreateInstance();//シェーダーファクトリーを生成する
	SHADER_FAC.init(g_pd3dDevice, g_pImmediateContext);//シェーダーファクトリーの初期化
	//=============================================================
	//描画機能に付随するほかのクラスを生成
	//=============================================================

	m_pBaseShading = new BASE_SHADING();
	DirectWrite::CreateInstance();
	//=============================================================
	//描画機能に付随するほかのクラスをここで初期化
	//=============================================================
	// BaseShadingの初期化
	hr = m_pBaseShading->Init((int)eGenerateType::FROM_FILE, g_pd3dDevice, g_pImmediateContext);
	if (FAILED(hr))return hr;


	//DirecctWriteの初期化
	hr = DWRITE.Initialize(g_pSwapChain);
	if (FAILED(hr))return hr;

	/*
	HANDLE handle;
	HRSRC hRes;
	tCSO cso;

	//コンパイル済みシェーダーファイルをリソースから読み込む
	hRes = FindResource(NULL, MAKEINTRESOURCE(FXID_TESTEFFECT), L"RCDATA");
	if (hRes == NULL)
	{
		return -1;
	}
	handle = LoadResource(NULL, hRes);
	if (!handle) {
		return -1;
	}
	cso.pData = (LPBYTE)LockResource(handle);
	cso.Size = SizeofResource(NULL, hRes);
	*/

	//========================================
	// その他の初期化事項 
	//========================================
	//バックバッファー初期化
	DIRECT3D11::InitBackBuffer();


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
	hr = g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_pRenderTargetView);
	pBackBuffer->Release();
	if (FAILED(hr))
		return hr;

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


	//描画先を設定
	g_pImmediateContext->OMSetRenderTargets(1, &g_pRenderTargetView, m_pDepthStencilView);

	//ビューポートを設定
	vp[0].Width = (FLOAT)descBackBuffer.Width;
	vp[0].Height = (FLOAT)descBackBuffer.Height;
	vp[0].MinDepth = 0.0f;
	vp[0].MaxDepth = 1.0f;
	vp[0].TopLeftX = 0;
	vp[0].TopLeftY = 0;

	g_pImmediateContext->RSSetViewports(1, &vp[0]);

	/*
	vp[1].Width = (FLOAT)descBackBuffer.Width/2;
	vp[1].Height = (FLOAT)descBackBuffer.Height;
	vp[1].MinDepth = 0.0f;
	vp[1].MaxDepth = 1.0f;
	vp[1].TopLeftX = (FLOAT)descBackBuffer.Width / 2;
	vp[1].TopLeftY = 0;
	*/


	return S_OK;
}

//
//
//
void DIRECT3D11::Clear()
{
	//画面クリア（実際は単色で画面を塗りつぶす処理）
	float ClearColor[4] = { 0,0,0,1 };// クリア色作成　RGBAの順
	g_pImmediateContext->ClearRenderTargetView(g_pRenderTargetView, ClearColor);//画面クリア
	g_pImmediateContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);//深度バッファクリア
}
//
//
//
HRESULT DIRECT3D11::Present(bool vSync, bool flag)
{
	g_pSwapChain->Present(vSync, flag);//画面更新（バックバッファをフロントバッファに）
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

// ブレンド ステートを無効にするための設定を取得する
void DIRECT3D11::SetDefaultBlendDesc(int value)
{
	m_pBaseShading->SetAlpha(g_pImmediateContext, value);
	BLENDSTATE blendState;
	D3D11_RENDER_TARGET_BLEND_DESC blendDesc;
	blendDesc = blendState.GetDefaultBlendDesc();
	blendState.SetBlendState(g_pd3dDevice, g_pImmediateContext, &blendDesc, 1, FALSE);
}
// 線形合成用ブレンド ステートのための設定を取得する
void DIRECT3D11::SetAlignmentBlendDesc(int value)
{
	m_pBaseShading->SetAlpha(g_pImmediateContext, value);
	BLENDSTATE blendState;
	D3D11_RENDER_TARGET_BLEND_DESC blendDesc;
	blendDesc = blendState.GetAlignmentBlendDesc();
	blendState.SetBlendState(g_pd3dDevice, g_pImmediateContext, &blendDesc, 1, FALSE);
}
// 加算合成用ブレンド ステートのための設定を取得する
void DIRECT3D11::SetAddBlendDesc(int value)
{
	m_pBaseShading->SetAlpha(g_pImmediateContext, value);
	BLENDSTATE blendState;
	D3D11_RENDER_TARGET_BLEND_DESC blendDesc;
	blendDesc = blendState.GetAddBlendDesc();
	blendState.SetBlendState(g_pd3dDevice, g_pImmediateContext, &blendDesc, 1, FALSE);
}
// 減算合成用ブレンド ステートのための設定を取得する
void DIRECT3D11::SetSubtractBlendDesc(int value)
{
	m_pBaseShading->SetAlpha(g_pImmediateContext, value);
	BLENDSTATE blendState;
	D3D11_RENDER_TARGET_BLEND_DESC blendDesc;
	blendDesc = blendState.GetSubtractBlendDesc();
	blendState.SetBlendState(g_pd3dDevice, g_pImmediateContext, &blendDesc, 1, FALSE);
}
// 積算合成用ブレンド ステートのための設定を取得する
void DIRECT3D11::SetMultipleBlendDesc(int value)
{
	m_pBaseShading->SetAlpha(g_pImmediateContext, value);
	BLENDSTATE blendState;
	D3D11_RENDER_TARGET_BLEND_DESC blendDesc;
	blendDesc = blendState.GetMultipleBlendDesc();
	blendState.SetBlendState(g_pd3dDevice, g_pImmediateContext, &blendDesc, 1, FALSE);
}

void DIRECT3D11::DrawBox(float x, float y, float w, float h, float* color)//クアッドを描画する
{

	Vertex2D v[4] = {
			{{x , y + h , _layer},{color[0],color[1],color[2],color[3]}, {0, 1}},	// 左下
			{{x , y , _layer}, {color[0],color[1],color[2],color[3]},{0, 0}},	// 左上
			{{x + w , y + h , _layer}, {color[0],color[1],color[2],color[3]},{1, 1}},	// 右下
			{{x + w , y , _layer}, {color[0],color[1],color[2],color[3]},{1, 0}},	// 右上
	};
	//-----------------------------  
	// シェーダーをセット
	//----------------------------
	g_pImmediateContext->VSSetShader(m_pBaseShading->m_pVertexShader, nullptr, 0);
	g_pImmediateContext->PSSetShader(m_pBaseShading->m_pPixelShader, nullptr, 0);
	//頂点情報をバッファに書き込む
	m_pBaseShading->WriteVertexInfo2D(g_pImmediateContext, v, sizeof(v));
	// インプットレイアウトの設定
	g_pImmediateContext->IASetInputLayout(m_pBaseShading->m_pLayout);

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

	//-----------------------------  
	// シェーダーをセット
	//----------------------------
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
	g_pImmediateContext->VSSetShader(m_pBaseShading->m_pVertexShader, nullptr, 0);
	g_pImmediateContext->PSSetShader(m_pBaseShading->m_pPixelShader, nullptr, 0);
	//頂点情報をバッファに書き込む
	m_pBaseShading->WriteVertexInfo2D(g_pImmediateContext, v, sizeof(v));
	// インプットレイアウトの設定
	g_pImmediateContext->IASetInputLayout(m_pBaseShading->m_pLayout);

	// プロミティブ・トポロジーをセット
	g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	g_pImmediateContext->Draw(4, 0);
}

void DIRECT3D11::DrawImage(const Texture* tex, float x, float y, float w, float h)//テクスチャ付きのクアッドを描画する
{

	if (tex == nullptr)
	{
		DrawBox(x, y, w, h, D3D.GetColor(255, 0, 255));
		return;
	};
	// 頂点データ作成
	Vertex2D v[4] = {
			{{x , y + h , _layer},{0,0,0,0}, {0, 1}},	// 左下
			{{x , y , _layer},{0,0,0,0}, {0, 0}},	// 左上
			{{x + w , y + h , _layer},{0,0,0,0}, {1, 1}},	// 右下
			{{x + w , y , _layer},{0,0,0,0}, {1, 0}},	// 右上
	};

	//-----------------------------
	// シェーダーをセット
	//----------------------------- 
	g_pImmediateContext->VSSetShader(m_pBaseShading->m_pVertexShader, nullptr, 0);
	g_pImmediateContext->PSSetShader(m_pBaseShading->m_pPixelShader2, nullptr, 0);

	// インプットレイアウトの設定
	g_pImmediateContext->IASetInputLayout(m_pBaseShading->m_pLayout);

	//頂点情報をバッファに書き込む
	m_pBaseShading->WriteVertexInfo2D(g_pImmediateContext, v, sizeof(v));

	// テクスチャを、スロット0にセット
	g_pImmediateContext->PSSetShaderResources(0, 1, tex->m_srv.GetAddressOf());

	// プロミティブ・トポロジーをセット
	g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);


	// デバイスコンテキストくん、上記のセットした内容で描画してください
	g_pImmediateContext->Draw(4, 0);
}

void DIRECT3D11::DrawSRVTex(ID3D11ShaderResourceView* const* srv, float x, float y, float w, float h)//テクスチャ付きのクアッドを描画する
{

	// 頂点データ作成
	Vertex2D v[4] = {
			{{x , y + h , 0},{1,1,1,1}, {0, 1}},	// 左下
			{{x , y , 0},{1,1,1,1}, {0, 0}},	// 左上
			{{x + w , y + h , 0},{1,1,1,1}, {1, 1}},	// 右下
			{{x + w , y , 0},{1,1,1,1}, {1, 0}},	// 右上
	};
	/*
	g_pImmediateContext->OMSetRenderTargets(1,&g_pRenderTargetView,nullptr );
	*/
	//-----------------------------
	// シェーダーをセット
	//----------------------------- 
	g_pImmediateContext->VSSetShader(m_pBaseShading->m_pVertexShader, nullptr, 0);
	g_pImmediateContext->PSSetShader(m_pBaseShading->m_pPixelShader2, nullptr, 0);

	// インプットレイアウトの設定
	g_pImmediateContext->IASetInputLayout(m_pBaseShading->m_pLayout);

	//頂点情報をバッファに書き込む
	m_pBaseShading->WriteVertexInfo2D(g_pImmediateContext, v, sizeof(v));

	// テクスチャを、スロット0にセット
	g_pImmediateContext->PSSetShaderResources(0, 1, srv);

	// プロミティブ・トポロジーをセット
	g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	g_pImmediateContext->Draw(4, 0);

	//g_pImmediateContext->OMSetRenderTargets(1, &g_pRenderTargetView, m_pDepthStencilView);
}

void DIRECT3D11::DrawRotImage(const Texture* tex, float x, float y, float w, float h, float angle, float size)//テクスチャ付きの回転するクアッドを描画する
{
	Vertex2D v[4] = {
			{{-(w / 2) , (h / 2) , 0},{0,0,0,0}, {0, 1}},	// 左下
			{{-(w / 2) , -(h / 2) , 0}, {0,0,0,0},{0, 0}},	// 左上
			{{(w / 2) , (h / 2) , 0}, {0,0,0,0},{1, 1}},	// 右下
			{{(w / 2) , -(h / 2) , 0}, {0,0,0,0},{1, 0}},	// 右上
	};

	//-----------------------------  
	// シェーダーをセット
	//----------------------------

	float POSX[4];
	float POSY[4];
	//角度を行列に代入
	for (int i = 0; i < 4; i++)
	{
		POSX[i] = (v[i].Pos.x * size) * cos(angle) - (v[i].Pos.y * size) * sin(angle);
		POSY[i] = (v[i].Pos.x * size) * sin(angle) + (v[i].Pos.y * size) * cos(angle);

		v[i].Pos.x = POSX[i] + x;
		v[i].Pos.y = POSY[i] + y;
	}// インプットレイアウトの設定
	g_pImmediateContext->IASetInputLayout(m_pBaseShading->m_pLayout);

	g_pImmediateContext->VSSetShader(m_pBaseShading->m_pVertexShader, nullptr, 0);
	g_pImmediateContext->PSSetShader(m_pBaseShading->m_pPixelShader2, nullptr, 0);
	//頂点情報をバッファに書き込む
	m_pBaseShading->WriteVertexInfo2D(g_pImmediateContext, v, sizeof(v));
	// テクスチャを、スロット0にセット
	g_pImmediateContext->PSSetShaderResources(0, 1, tex->m_srv.GetAddressOf());
	// プロミティブ・トポロジーをセット
	g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	g_pImmediateContext->Draw(4, 0);
}

void DIRECT3D11::DrawDivImage(const Texture* tex, float x, float y, float w, float h, float xi, float yi, int xSize, int ySize)//分割テクスチャ付きのクアッドを描画する
{

	size_t imageXSize = tex->m_info.width;
	size_t imageYSize = tex->m_info.height;
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

	// インプットレイアウトの設定
	g_pImmediateContext->IASetInputLayout(m_pBaseShading->m_pLayout);

	g_pImmediateContext->VSSetShader(m_pBaseShading->m_pVertexShader, nullptr, 0);
	g_pImmediateContext->PSSetShader(m_pBaseShading->m_pPixelShader2, nullptr, 0);
	//頂点情報をバッファに書き込む
	m_pBaseShading->WriteVertexInfo2D(g_pImmediateContext, v, sizeof(v));
	// テクスチャを、スロット0にセット
	g_pImmediateContext->PSSetShaderResources(0, 1, tex->m_srv.GetAddressOf());
	// プロミティブ・トポロジーをセット
	g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	g_pImmediateContext->Draw(4, 0);
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

void DIRECT3D11::RemoveDevice()
{
	ShaderFactory::DeleteInstance();
	DirectWrite::DeleteInstance();
	SAFE_DELETE(m_pBaseShading);

	if (g_pImmediateContext) g_pImmediateContext->ClearState();
	SAFE_RELEASE(m_pBlendState);
	SAFE_RELEASE(m_pDepthStencilView);
	SAFE_RELEASE(g_pRenderTargetView);
	SAFE_RELEASE(m_pDepthStencilTexture);
	SAFE_RELEASE(g_pSwapChain1);
	SAFE_RELEASE(g_pSwapChain);
	SAFE_RELEASE(g_pImmediateContext1);
	SAFE_RELEASE(g_pImmediateContext);
	SAFE_RELEASE(g_pd3dDevice1);
	SAFE_RELEASE(g_pd3dDevice);

}