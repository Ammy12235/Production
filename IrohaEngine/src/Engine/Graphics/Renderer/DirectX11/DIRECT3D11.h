#pragma once
#include "Core/Singleton.h"
#include "Core/window/WINDOW.h"
#include "Core/DEFINE.h"

//描画関連
#include "BaseShading.h"
#include "Graphics/Texture/TextureFactory.h"
#include "Graphics/Shader/ShaderFactory.h"
#include "Graphics/Shader/ShaderBlob.h"
#include "SamplerState.h"
#include "BlendState.h"
#include "Graphics/Renderer/DirectX11/DrawCommand/eBlendState.h"
#include "DIRECTWRITE.h"

//レイヤー機能関連
#include "Graphics/Renderer/DirectX11/DrawCommand/DrawStorage.h"
#include "Graphics/Renderer/DirectX11/DrawCommand/eLayer.h"
#include "Graphics/Renderer/DirectX11/DrawCommand/eDrawCommand.h"

//ポストエフェクト関連
#include "Graphics/PostEffect/PostEffectChain.h"
#include "Graphics/PostEffect/HSVFilter.h"
#include "Graphics/PostEffect/DiskBlurFilter.h"
#include "Graphics/PostEffect/GaussianBlurFilter.h"
#include "Graphics/PostEffect/DropletFilter.h"


//
//
class TextureFactory;
class ShaderFactory;
class BASE_SHADING;
class Instancing;

class DrawStorage;

class PostEffectChain;
class HSVFilter;
class DiskBlurFilter;
class GaussianBlurFilter;
class DropletFilter;

//
//
//Direct3D11クラス:各DirectX11描画用デバイスの初期化、管理、削除を行うクラス。また、追加機能の管理も行う
class DIRECT3D11 : public Singleton<DIRECT3D11>
{
public:

	//===================================
	//初期化用関数
	//===================================
	HRESULT Init(D3D_INIT* pcd);
	//===================================
	//終了用関数
	//===================================
	void RemoveDevice();

	//===================================
	//設定用関数
	//===================================
	HRESULT SetGamma(float gamma);
	HRESULT SetFullScreen(bool isFullScreen);
	//===================================
	//スクリーンの更新と削除
	//===================================
	void Clear();
	HRESULT Present();
	//===================================
	//多角形描画関数
	//===================================
	void DrawPoint(float x, float y, float* color);                                                         //点を描画する
	void DrawLine(float x1, float y1, float x2, float y2, float* color);                                    //線を描画する
	void DrawBox(float x, float y, float w, float h, float* color);                           //クアッドを描画する
	void DrawRotBox(float x, float y, float w, float h, float angle, float size, float* color);//クアッドを描画する(回転、拡大あり)

	void DrawImagebyName(std::string texName, float x, float y, float w, float h);                             //テクスチャ付きのクアッドをテクスチャ名から描画する
	void DrawImage(int texId, float x, float y, float w, float h);                                             //テクスチャ付きのクアッドを描画する
	void DrawImageFlip(int texId, float x, float y, float w, float h, bool isFlip);                            //テクスチャ付きのクアッドを描画する(左右反転あり)
	void DrawRotImage(int texId, float x, float y, float w, float h, float angle, float size);                 //テクスチャ付きクアッドを描画する(回転、拡大あり)
	void DrawRotFlipImage(int texId, float x, float y, float w, float h, float angle, float size,bool isFlip);//テクスチャ付きの回転するクアッドを描画する
	void DrawExtendedImage(int texId, float x, float y, float w, float h, float xSize, float ySize);           //テクスチャ付きクアッドを描画する(x,y軸にそれぞれ個別に拡大)
	void DrawDivImage(int texId, float x, float y, float w, float h, float xi, float yi, int xSize, int ySize);//１つのテクスチャを等間隔で分割し、テクスチャ付きのクアッドを描画する
	void DrawSRVTex(ID3D11ShaderResourceView* const* srv, float x, float y, float w, float h);     //テクスチャ付きのクアッドを描画する
	void DrawInstancedImage(int texId, float x, float y,               //テクスチャ付きクアッドをインスタンス描画する。
		const InstanceData2D& data,size_t dataSize, int mapSizeX,int mapSizeY);
	//===================================
	//3D構造物
	//===================================
	void DrawCube(float posX, float posY, float posZ,
		float rotX, float rotY, float rotZ,
		float sclX, float sclY, float sclZ);
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

	// ブレンドステートを設定する
	void SetBlendDesc(eBlendState blendstate, int value);
	//色を作る
	float* GetColor(int r, int g, int b);

	//===================================
	// サンプラーステート
	//===================================
	//通常のサンプラーステートを設定
	void SetDefaultSampler();
	//遷移エフェクトで使用するサンプラーステートを設定
	void SetFocusHoleSampler();


	//===================================
	// ビューポート設定
	//===================================
	void SetViewportId(int id);
	ID3D11Device* GetDevice()
	{
		return g_pd3dDevice;
	}
	ID3D11DeviceContext* GetDeviceContext()
	{
		return g_pImmediateContext;
	}
	//===================================
	// 深度ステンシル設定
	//===================================
	void IsSetDepthStencil(bool isApply);
	//===================================
	// レイヤー機能
	//===================================
	void setIsApplyLayer(bool isApply)
	{
		_isApplyLayer = isApply;
	}
	void SetLayer(eLayer layer);
	void SetIsDrawLayer(bool isDraw, eLayer layer)
	{
		m_pDrawStorage->setDrawFlag(isDraw, layer);
	}
	void ExecuteDraw();
	void ExecuteDrawCameraUI();

	
	//===================================
	// ポストエフェクト
	//===================================
	void setIsApplyPostEffect(bool isApply)
	{
		_isPostProcessing = isApply;
	}
	bool getIsApplyPostEffect()const
	{
		return _isPostProcessing;
	}
	void ApplyPostProcessing();

	HSVFilter* getHSVFilter()const;
	DiskBlurFilter* getDiskBlurFilter()const;
	GaussianBlurFilter* getGaussianBlurFilter()const;
	DropletFilter* getDropletFilter()const;


private:
	//===================================
	//各機能初期化用関数
	//===================================
	HRESULT InitBackBuffer();//バックバッファの初期化
	HRESULT InitBaseShader();//基本描画用シェーダの初期化
	bool InitLayer();        //レイヤー機能の初期化
	bool InitPostEffect();   //ポストエフェクトの初期化
	bool InitEffect();       //エフェクトの初期化
	//===================================
	//カメラ行列
	//===================================
	XMVECTOR Eye;
	XMVECTOR At;
	XMVECTOR Up;

	eBlendState eblendState = BlendState_None;
	eLayer elayer = Layer_0;

	//Data
	HWND m_hWnd = nullptr;
	ID3D11Device* g_pd3dDevice = nullptr;
	ID3D11DeviceContext* g_pImmediateContext = nullptr;
	IDXGISwapChain* g_pSwapChain = nullptr;
	ID3D11BlendState* m_pBlendState = nullptr;

	ID3D11RenderTargetView* g_pInputRTV;//ポストエフェクトを適用するための入力RTV
	ID3D11RenderTargetView* g_pBackBufferRTV = nullptr;//最終結果を表示させるための出力RTV

	ID3D11DepthStencilView* m_pDepthStencilView = nullptr;

	ID3D11Device1* g_pd3dDevice1 = nullptr;
	ID3D11DeviceContext1* g_pImmediateContext1 = nullptr;
	IDXGISwapChain1* g_pSwapChain1 = nullptr;
	ID3D11Texture2D* m_pDepthStencilTexture = nullptr;

	D3D11_VIEWPORT vp[2] = { 0 };

	ID3D11RasterizerState* m_pRasterizerState = nullptr;

	D3D_DRIVER_TYPE         g_driverType = D3D_DRIVER_TYPE_NULL;
	D3D_FEATURE_LEVEL       g_featureLevel = D3D_FEATURE_LEVEL_11_0;

	UINT viewWidth = 0;
	UINT viewHeight = 0;

	float color[4] = { 0 };

	float errorColor[4] = { 1.0f,0.0f,1.0f,1.0f };//テクスチャ読み込みが失敗したときに表示する色

	bool _isPostProcessing = false;
	bool _isApplyLayer = false;

	//シェーダークラス
	BASE_SHADING* m_pBaseShading = nullptr;
	Instancing* m_pInstancing = nullptr;

	//レイヤー機能クラス
	DrawStorage* m_pDrawStorage = nullptr;

	//ポストエフェクト管理クラス
	PostEffectChain* m_pPostEffectChain = nullptr;

	//ポストエフェクト群
	HSVFilter* m_pHSVFilter = nullptr;
	DiskBlurFilter* m_pDiskBlurFilter = nullptr;
	GaussianBlurFilter* m_pGaussianBlurFilter = nullptr;
	DropletFilter* m_pDropletFilter = nullptr;


	//===================================
	//ガンマ補正
	//===================================
	DXGI_GAMMA_CONTROL_CAPABILITIES gammacap;
	DXGI_GAMMA_CONTROL gammacontrol;
public:
	//Method
	DIRECT3D11()
	{

	};
	virtual ~DIRECT3D11()
	{
		RemoveDevice();
	};
};

#define D3D DIRECT3D11::GetInstance()
