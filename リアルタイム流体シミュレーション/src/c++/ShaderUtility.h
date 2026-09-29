#pragma once
#include "Base.h"
#include "DirectX.h"

//==========================================
//頂点バッファ関連
//==========================================

//ポストエフェクト用頂点バッファ
typedef struct ALIGN16 _VERTEX
{
	XMFLOAT3 pos;// 頂点座標
	XMFLOAT2 texel;// テクセル
}VERTEX;

class ShaderUtility
{
public:
	ShaderUtility() = default;
	virtual ~ShaderUtility() = default;

	HRESULT CompileShaderFromFile(const WCHAR* szFileName, LPCSTR szEntryPoint,//シェーダーファイルをコンパイルする
		LPCSTR szShaderModel, ID3DBlob** ppBlobOut);
	HRESULT CreateRenderTargetView(ID3D11Device* pD3DDevice, ID3D11RenderTargetView** ppRTView, UINT Width, UINT Height, DXGI_FORMAT Format) const;//RTVを作成する
	//HRESULT Create(ID3D11Device* pD3DDevice, ID3D11Texture2D** ppTexture, UINT Width, UINT Height, DXGI_FORMAT Format) const;//Texture2Dを作成する

	ID3D11ShaderResourceView* GetSRViewFromRTView(ID3D11Device* pD3DDevice, ID3D11RenderTargetView* pRTView) const;                                //RTVからSRVを作成する
	ID3D11ShaderResourceView* GetSRViewFromTexture(ID3D11Device* pD3DDevice, ID3D11Texture2D* pTex2D) const;                                //Texture2DからSRVを作成する
	XMFLOAT2 GetRTViewSize(ID3D11RenderTargetView* pRTView) const;  //RTVの縦横のサイズを取得
	XMFLOAT2 GetSRViewSize(ID3D11ShaderResourceView* pSRView) const;//SRVの縦横のサイズを取得

private:
	HRESULT CreateBuffer(ID3D11Device* pD3DDevice, ID3D11Buffer** pBuffer, void* pData, size_t size, UINT CPUAccessFlag, D3D11_BIND_FLAG BindFlag);//バッファを作成

public:
	HRESULT CreateVertexBuffer(ID3D11Device* pD3DDevice, ID3D11Buffer** pBuffer, void* pData, size_t size, UINT CPUAccessFlag);  // 頂点バッファを作成する
	HRESULT CreateIndexBuffer(ID3D11Device* pD3DDevice, ID3D11Buffer** pBuffer, void* pData, size_t size, UINT CPUAccessFlag);   // インデックスバッファを作成する
	HRESULT CreateConstantBuffer(ID3D11Device* pD3DDevice, ID3D11Buffer** pBuffer, void* pData, size_t size, UINT CPUAccessFlag);// 定数バッファを作成する

};
