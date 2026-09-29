#pragma once
#include "PostEffect.h"
class PostEffect;

//水の落下フィルタークラス：雨の時に滴る水滴を表現するクラス。
class DropletFilter :public PostEffect
{
private:
	// 水滴フィルターの定数バッファ定義
	typedef struct ALIGN16 _CBUFFER
	{
		XMFLOAT2 offset;
		XMFLOAT2 addDynamicDropletPos;
		XMFLOAT2 addStaticDropletPos;
		float distortion;
		float attenuate;
	}CBUFFER;


	static const int numPS = 8;

	//デバイス、デバイスコンテキスト
	ID3D11Device* m_pD3DDevice = nullptr;
	ID3D11DeviceContext* m_pD3DDeviceContext = nullptr;

	ID3D11InputLayout* m_pLayout = nullptr;	// 入力レイアウト
	ID3D11Buffer* m_pVertexBuffer = nullptr;//頂点バッファ
	ID3D11Buffer* m_pConstantBuffer = nullptr;// 定数バッファ
	ID3D11VertexShader* m_pVertexShader = nullptr;// 頂点シェーダー
	ID3D11PixelShader* m_pPixelShader[numPS];// ピクセルシェーダー
	ID3D11SamplerState* m_pSamplerState = nullptr;// サンプラーステート

	// レンダーターゲット用のリソース
	// 
//水滴マップ（動的）
	ID3D11RenderTargetView* g_pDropletsRTV[2] = { NULL };
	ID3D11Texture2D* g_pDropletsTex[2] = { NULL };
	ID3D11ShaderResourceView* g_pDropletsSRV[2] = { NULL };

	//水滴マップ(静的)
	ID3D11RenderTargetView* g_pDropletsRTV2[2] = { NULL };
	ID3D11Texture2D* g_pDropletsTex2[2] = { NULL };
	ID3D11ShaderResourceView* g_pDropletsSRV2[2] = { NULL };

	//水滴の軌跡マップ
	ID3D11RenderTargetView* g_pDropletsLocusRTV[2] = { NULL };
	ID3D11Texture2D* g_pDropletsLocusTex[2] = { NULL };
	ID3D11ShaderResourceView* g_pDropletsLocusSRV[2] = { NULL };

	//結合マップ
	ID3D11RenderTargetView* g_pMergeRTV = { NULL };
	ID3D11Texture2D* g_pMergeTex = { NULL };
	ID3D11ShaderResourceView* g_pMergeSRV = { NULL };

	//ブラー適応
	ID3D11RenderTargetView* g_pBlurRTV = NULL;
	ID3D11Texture2D* g_pBlurTex = NULL;
	ID3D11ShaderResourceView* g_pBlurSRV = NULL;

	//法線マップ
	ID3D11RenderTargetView* g_pNormalRTV = NULL;
	ID3D11Texture2D* g_pNormalTex = NULL;
	ID3D11ShaderResourceView* g_pNormalSRV = NULL;

	int m_pRTVTargetIndex = 0;//水滴マップの参照インデックス
	ID3D11DepthStencilView* g_pDSV = NULL; // 深度ステンシルビュー
	D3D11_VIEWPORT g_pVp;//ビューポート

	// 縮小バッファの解像度
	UINT m_Width;
	UINT m_Height;

	bool _isAddDynamic = false;
	bool _isAddStatic= false;
	XMFLOAT2 _addDynamicDropletPos = { 0.0f,0.0f };
	XMFLOAT2 _addStaticDropletPos = { 0.0f,0.0f };
	int _frictionTextureId = 0;
	float _distortion = 0.0f;
	float _attenuate=0.0f;

	LPCSTR ps_main[numPS] =
	{
		"PS_AddDynamicDroplets",
		"PS_NoneAddDynamicDroplets",
		"PS_AddStaticDroplets",
		"PS_NoneAddStaticDroplets",
		"PS_MergeDroplets",
		"PS_Blur",
		"PS_CreateNormalMap",
		"PS_Distortion"
	};

	enum ePS
	{
		PS_None = -1,
		PS_AddDynamicDroplets,
		PS_NoneAddDynamicDroplets,
		PS_AddStaticDroplets,
		PS_NoneAddStaticDroplets,
		PS_MergeDroplets,
		PS_Blur,
		PS_CreateNormalMap,
		PS_Distortion
	};
	HRESULT setConstantBuffers(float distortion, float attenuate, XMFLOAT2 addDynamicDropletPos, XMFLOAT2 addStaticDropletPos, bool isAddDynamic, bool isAddStatic);

public:
	bool init(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int width, int height)override;//初期化
	void apply(IN ID3D11RenderTargetView* inputRTV, OUT ID3D11RenderTargetView* outputRTV)override;
	void setParameter(float distortion, float attenuate, XMFLOAT2 addDynamicDropletPos, XMFLOAT2 addStaticDropletPos, bool isAddDynamic, bool isAddStatic);
	void cleanup()override;
	DropletFilter();
	virtual ~DropletFilter();

};
