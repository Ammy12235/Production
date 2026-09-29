#pragma once
#include "DirectX.h"

#define ALIGN16 _declspec(align(16)) 

class DROPWATERFILTER
{
private:
	// ブラーフィルターの定数バッファ定義
	typedef struct ALIGN16 _CBUFFER
	{
		XMFLOAT2 offset;
		XMFLOAT2 addDynamicDropletPos;
		XMFLOAT2 addStaticDropletPos;
		float distortion;
		float attenuate;
	}CBUFFER;

	static const int numPS = 8;

	//デバイス
	ID3D11Device* pD3DDevice = nullptr;
	ID3D11DeviceContext* pD3DDeviceContext = nullptr;

	// 入力レイアウト
	ID3D11InputLayout* m_pLayout = nullptr;
	//頂点バッファ
	ID3D11Buffer* m_pVertexBuffer = nullptr;
	// 定数バッファ
	ID3D11Buffer* m_pConstantBuffers = nullptr;
	// 頂点シェーダー
	ID3D11VertexShader* m_pVertexShader = nullptr;
	// ピクセルシェーダー
	ID3D11PixelShader* m_pPixelShader[numPS] = { nullptr };
	// サンプラーステート
	ID3D11SamplerState* m_pSamplerState = nullptr;


	static const int numRTV = 4;
	// レンダーターゲット用のリソース

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

	bool isAddDynamicDropletWater = false;
	bool isAddStaticDropletWater = false;

	LPCSTR vs_main = "VS";
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
		PS_None=-1,
		PS_AddDynamicDroplets,
		PS_NoneAddDynamicDroplets,
		PS_AddStaticDroplets,
		PS_NoneAddStaticDroplets,
		PS_MergeDroplets,
		PS_Blur,
		PS_CreateNormalMap,
		PS_Distortion
	};

	ID3D11ShaderResourceView* GetSRViewFromRTView(ID3D11Device* pD3DDevice, ID3D11RenderTargetView* pRTView) const;
	void RemoveDevice();
	HRESULT CompileShaderFromFile(const WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);
public:
	DROPWATERFILTER();
	~DROPWATERFILTER();

	// 初期化
	HRESULT Init(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dDeviceContext, TCHAR pSrcFile[], UINT Width, UINT Height);
	// 定数バッファを設定する
	HRESULT SetConstantBuffers(float distortion, float attenuate, XMFLOAT2 addDynamicDropletPos, XMFLOAT2 addStaticDropletPos, bool isAddDynamic,bool isAddStatic);
	// 描画
	HRESULT Render(
		IN  ID3D11RenderTargetView* pInRTView,
		OUT ID3D11RenderTargetView* pOutRTView);
};