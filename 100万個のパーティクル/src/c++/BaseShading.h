#pragma once
#include "DirectX.h"

#define ALIGN16 _declspec(align(16)) 
//2D用頂点バッファ
struct Vertex2D
{
	XMFLOAT3 Pos;
	XMFLOAT4 Color;
	XMFLOAT2 Uv;
};

class BASE_SHADING
{
private:


	// 頂点シェーダー用の定数バッファ定義
	typedef struct _CBUFFER0
	{
		// ワールド × ビュー × 射影 行列
		ALIGN16 XMMATRIX  matWVP;
		ALIGN16 UINT viewPortWidth;
		ALIGN16 UINT viewPortHeight;
		ALIGN16 float alpha;

	}CBUFFER0;

	// 入力レイアウト
	ID3D11InputLayout* m_pLayout = nullptr;
	//頂点バッファ
	ID3D11Buffer* m_pVertexBuffer =  nullptr;
	// 定数バッファ
	ID3D11Buffer* m_pConstantBuffer =  nullptr;
	/*
	// 動的シェーダーリンク
	// ID3D11ClassLinkage
	ID3D11ClassLinkage* m_pPSClassLinkage = nullptr;
	//シェーダーリフレクション
	ID3D11ShaderReflection* pReflector = nullptr;
	//シェーダーファイルのインターフェイスインスタンスの数
	UINT g_iNumPSInterfaces = 0;
	ID3D11ClassInstance** g_dynamicLinkageArray = nullptr;
	UINT g_iAmbientLightingOffset = 0;
	ID3D11ShaderReflectionVariable* pAmbientLightingVar = nullptr;
	// ピクセルシェーダーに設定時に使用するクラスインスタンス
	// ID3D11ClassInstance
	ID3D11ClassInstance* m_pClassInstance = nullptr;
	*/
	//シェーダーファイル名指定
	WCHAR hlslSrc[100] = L"BaseShading.hlsl";
	LPCSTR vs_main = "VS";
	LPCSTR ps_main_01 = "PS";
	LPCSTR ps_main_02 = "PS2";

	//===================================
	//計算用行列
	//===================================
	XMMATRIX                g_World;
	float alpha = 0;
public:
	// 頂点シェーダー
	ID3D11VertexShader* m_pVertexShader = nullptr;
	// ピクセルシェーダー
	ID3D11PixelShader* m_pPixelShader = nullptr;
	ID3D11PixelShader* m_pPixelShader2 = nullptr;

	//シェーダーをコンパイルする
	HRESULT CompileShaderFromFile(const WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);

	// 初期化
	HRESULT InitPreCom(ID3D11Device* pD3DDevice, ID3D11DeviceContext* pD3DDeviceContext
		, const BYTE* pVS, size_t VSSize
		, const BYTE* pPS, size_t PSSize
	);
	HRESULT Init(ID3D11Device* pD3DDevice, ID3D11DeviceContext* pD3DDeviceContext);
	HRESULT ChangeMode_2D(ID3D11Device* pD3DDevice,ID3D11DeviceContext* pD3DDeviceContext);
	void WriteVertexInfo2D(ID3D11DeviceContext* pD3DDeviceContext,Vertex2D* v, int arraySize);
	void WriteCameraInfo2D(ID3D11DeviceContext* pD3DDeviceContext,XMFLOAT2 pos, float rot, XMFLOAT2 scl);
	void SetAlpha(ID3D11DeviceContext* pD3DDeviceContext,int value);

	// 描画終了処理
	void CleanupDevice();

	BASE_SHADING();
	~BASE_SHADING();
};