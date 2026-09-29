#pragma once
#include "DirectX.h"
#include <random>
#define ALIGN16 _declspec(align(16)) 
class Instancing
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

	//2D用頂点バッファ
	struct Vertex2D
	{
		XMFLOAT3 Pos;
		XMFLOAT2 Uv;
	};

	struct InstanceData
	{
		XMFLOAT3 pos;
		XMFLOAT2 uvOffset;
	};

	// 入力レイアウト
	ID3D11InputLayout* m_pLayout = nullptr;
	//頂点バッファ
	ID3D11Buffer* m_pVertexBuffer = nullptr;
	ID3D11Buffer* m_pInstanceBuffer = nullptr;
	// 定数バッファ
	ID3D11Buffer* m_pConstantBuffer = nullptr;
	// 頂点シェーダー
	ID3D11VertexShader* m_pVertexShader = nullptr;
	// ピクセルシェーダー
	ID3D11PixelShader* m_pPixelShader = nullptr;
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
	WCHAR hlslSrc[100] = L"Instancing.hlsl";
	LPCSTR vs_main = "VS";
	LPCSTR ps_main_01 = "PS";

	//===================================
	//計算用行列
	//===================================
	XMMATRIX                g_World;
	float alpha = 1;

	ID3D11Device* m_pDevice = nullptr;
	ID3D11DeviceContext* m_pDeviceContext = nullptr;
	static const int instSize = 1024;
	static const int maxInstanceCount = instSize * instSize;
	int mapWidth = 32;
	int mapHeight = 32;

	float size = 64;

	InstanceData instances[maxInstanceCount];
	std::random_device rd;
public:

	//シェーダーをコンパイルする
	HRESULT CompileShaderFromFile(const WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);

	// 初期化
	HRESULT Init(ID3D11Device* pD3DDevice, ID3D11DeviceContext* pD3DDeviceContext);
	void WriteVertexInfo2D(ID3D11DeviceContext* pD3DDeviceContext, Vertex2D* v, int arraySize);
	void WriteCameraInfo2D(ID3D11DeviceContext* pD3DDeviceContext, XMFLOAT2 pos, float rot, XMFLOAT2 scl);
	void SetAlpha(ID3D11DeviceContext* pD3DDeviceContext, int value);
	// 描画終了処理
	void CleanupDevice();

	void DrawInstancedBox(const Texture* tex,int xCount, int yCount, int mapSize);

	Instancing()=default;
	virtual ~Instancing()
	{
		CleanupDevice();
	};
};