#pragma once
#include "BASE.h"
#include "WINDOW.h"
#include "DirectX.h"

//
//
class TextureFactory;
class BASE_SHADING;

struct D3D_INIT
{
	HWND hWnd;
};
//
//
//Direcct3D11クラス:各描画用デバイスの初期化、管理、削除を行う。また、追加機能の管理も行う
class DIRECT3D11 final : public CELEMENT
{
public:

	//===================================
	//初期化用関数
	//===================================
	HRESULT Init(D3D_INIT*);
	HRESULT InitBackBuffer();
	//===================================
	//終了用関数
	//===================================
	void RemoveDevice();

	//===================================
	//設定用関数
	//===================================
	HRESULT SetGamma(float gamma);

	//===================================
	//スクリーンの更新と削除
	//===================================
	void Clear();
	HRESULT Present(bool vSync,bool flag);
	//===================================
	//多角形描画関数
	//===================================
	void DrawBox(float x, float y, float w, float h, float* color);
	void DrawRotBox(float x, float y, float w, float h,float angle, float size, float* color);//クアッドを描画する
	void DrawImage(const Texture* tex, float x, float y, float w, float h);//テクスチャ付きのクアッドを描画する
	void DrawSRVTex(ID3D11ShaderResourceView* const* srv, float x, float y, float w, float h);//テクスチャ付きのクアッドを描画する
	void DrawRotImage(const Texture* tex, float x, float y, float w, float h, float angle, float size);
	void DrawDivImage(const Texture* tex, float x, float y, float w, float h, float xi, float yi, int xSize, int ySize);
	//===================================
	//ブレンドモード
	//===================================
	// ブレンド ステートを無効にするための設定をする
	void SetDefaultBlendDesc(int value);
	// 線形合成用ブレンド ステートのためのを設定をする
	void SetAlignmentBlendDesc(int value);
	// 加算合成用ブレンド ステートのためのを設定をする
	void SetAddBlendDesc(int value);
	// 減算合成用ブレンド ステートのためのを設定をする
	void SetSubtractBlendDesc(int value);
	// 積算合成用ブレンド ステートのためのを設定をする
	void SetMultipleBlendDesc(int value);
	//色を作る
    float* GetColor(int r,int g,int b);
	ID3D11Device* GetDevice()
	{
		return g_pd3dDevice;
	}
	ID3D11DeviceContext* GetDeviceContext()
	{
		return g_pImmediateContext;
	}
	void SetLayer(float layer)
	{
		_layer = layer;
	}
private:
	float _layer=0;

	//===================================
	//カメラ行列
	//===================================
	XMVECTOR Eye;
	XMVECTOR At;
	XMVECTOR Up;

	enum eBlendState
	{
		Blend_None,
		Blend_Default,
		Blend_Alignment,
		Blend_Add,
		Blend_Subtract,
		Blend_Multiple,
		Blend_Max
	};

	eBlendState _blendstate;

	//Data
	HWND m_hWnd = nullptr;
	ID3D11Device* g_pd3dDevice = nullptr;
	ID3D11DeviceContext* g_pImmediateContext = nullptr;
	IDXGISwapChain* g_pSwapChain = nullptr;
	ID3D11BlendState* m_pBlendState = nullptr;
	ID3D11RenderTargetView* g_pRenderTargetView = nullptr;
	ID3D11DepthStencilView* m_pDepthStencilView = nullptr;
	ID3D11Texture2D* m_pBuckBuffer_DSTex = nullptr;
	ID3D11DepthStencilState* m_pBuckBuffer_DSTexState = nullptr;
	ID3D11Device1* g_pd3dDevice1 = nullptr;
	ID3D11DeviceContext1* g_pImmediateContext1 = nullptr;
	IDXGISwapChain1* g_pSwapChain1 = nullptr;
	ID3D11Texture2D* m_pDepthStencilTexture = nullptr;

	D3D11_VIEWPORT vp[2]={0};

	ID3D11RasterizerState* m_pRasterizerState=nullptr;

	D3D_DRIVER_TYPE         g_driverType = D3D_DRIVER_TYPE_NULL;
	D3D_FEATURE_LEVEL       g_featureLevel = D3D_FEATURE_LEVEL_11_0;

	UINT viewWidth=0;
	UINT viewHeight=0;

	float color[4]={0};

	//シェーダークラス
	BASE_SHADING* m_pBaseShading = nullptr;


	//===================================
	//ガンマ補正
	//===================================
	DXGI_GAMMA_CONTROL_CAPABILITIES gammacap;
	DXGI_GAMMA_CONTROL gammacontrol;

	//Method
	DIRECT3D11()
	{

	};
	virtual ~DIRECT3D11()
	{
		RemoveDevice();
	};



	DIRECT3D11(const  DIRECT3D11& r) = default;
	DIRECT3D11& operator=(const  DIRECT3D11& r) = default;

	static inline DIRECT3D11* s_instance;

public:


	static void CreateInstance() {
		if (!s_instance) {
			s_instance = new DIRECT3D11;
		}
	}

	static void DeleteInstance() {
		delete s_instance;
		s_instance = nullptr;
	}
	static DIRECT3D11& GetInstance() {
		return *s_instance;
	}

};

#define D3D DIRECT3D11::GetInstance()
