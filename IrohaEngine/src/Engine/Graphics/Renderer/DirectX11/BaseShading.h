#pragma once

#include "Core/Base.h"
#include "Graphics/Shader/ShaderBlob.h"
#include "Graphics/Renderer/DirectX11/DrawCommand/eBaseShading.h"

constexpr UINT MAX_InstancedIndexNum = 64 * 64;//同時に送信できるインスタンスデータ最大数
//ベースシェーディングクラス：ポリゴンの描画などの基本のシェーダークラス。
class BASE_SHADING
{
private:

	// 2D頂点シェーダー用の定数バッファ定義
	typedef struct ALIGN16 _CBUFFER_2D
	{
		// ワールド × ビュー × 射影 行列
		XMMATRIX  matWVP;
		UINT viewPortWidth;
		UINT viewPortHeight;
		float alpha;

	}CBUFFER_2D;

	// 3D頂点シェーダー用の定数バッファ定義
	typedef struct ALIGN16 _CBUFFER_3D
	{
		XMMATRIX mWorld;
		XMMATRIX mView;
		XMMATRIX mProjection;
		XMFLOAT4 light;
		XMFLOAT4 lightColor;
		XMFLOAT4 attenuation;
		XMFLOAT4 eyePos;
	}CBUFFER_3D;

	//3D頂点シェーダ用の定数バッファ定義

	//シェーダーコンテナ
	std::vector< ID3D11VertexShader* >		mVS;
	std::vector< ID3D11PixelShader* >		mPS;
	std::vector< ID3D11GeometryShader* >	mGS;
	std::vector< ID3D11HullShader* >		mHS;
	std::vector< ID3D11DomainShader* >		mDS;
	std::vector< ID3D11ComputeShader* >		mCS;
	std::vector< ID3D11InputLayout*>        mLayout;

	//バッファコンテナ
	std::vector <ID3D11Buffer*> mVBuffer;
	std::vector <ID3D11Buffer*> mCBuffer;
	std::vector <ID3D11Buffer*> mIBuffer;


	//===================================
	//計算用行列
	//===================================
	XMMATRIX                g_World;
	XMMATRIX                g_View;
	XMMATRIX                g_Projection;

	XMVECTOR Eye;
	XMVECTOR At;
	XMVECTOR Up;

	//アルファ値を保存する変数
	float alpha = 0;
	int _value = 0;


	ID3D11Device* _pD3DDevice = nullptr;
	ID3D11DeviceContext* _pD3DDeviceContext = nullptr;
public:

	//初期化（コンパイル作業を含む）
	HRESULT Init(ID3D11Device& pD3DDevice,ID3D11DeviceContext& pD3DDeviceContext);
	HRESULT ChangeMode_2D(ID3D11Device& pD3DDevice,ID3D11DeviceContext& pD3DDeviceContext);
	void WriteVertexInfo2D(ID3D11DeviceContext& pD3DDeviceContext,Vertex2D& v, size_t arraySize);
	void WriteVertexInfoInstancing2D(ID3D11DeviceContext& pD3DDeviceContext,
		InstanceData2D& v, size_t arraySize,const InstanceData2D& data, size_t dataSize);//もととなる頂点の形とインスタンスデータを受け取り、GPUにセット
	void WriteVertexInfo3D(ID3D11DeviceContext& pD3DDeviceContext,Vertex3D& v, size_t arraySize);
	void SetAlpha(int value);
	int GetAlpha()const;
	void SetShader(ID3D11DeviceContext& pImmediateContext,
		BASE_VERTEXSHADER indexVS,
		BASE_PIXELSHADER indexPS,
		int indexGS = -1,
		int indexHS = -1,
		int indexDS = -1,
		int indexCS = -1);
	void SetInputLayout(ID3D11DeviceContext& pImmediateContext, BASE_LAYOUT index);

	// 描画終了処理
	void CleanupDevice();
	void ReleseContainer();

	BASE_SHADING();
	~BASE_SHADING();
};


