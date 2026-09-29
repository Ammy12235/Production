#pragma once
#include "DirectX.h"

#define ALIGN16 _declspec(align(16)) 

// ２Ｄポリゴンの頂点定義
typedef struct _VERTEX
{
    // 頂点座標
    XMFLOAT3 pos;
    // テクセル
    XMFLOAT2 texel;
}VERTEX;

class GAUSSIANFILTER
{
private:
    // ブラーフィルターの定数バッファ定義
    typedef struct ALIGN16 _CBUFFER
    {
         float Weight[8];
         XMFLOAT2 Offset;
         float Width;
         float Height;
    }CBUFFER;

    float mWeights[8];

    // 入力レイアウト
    ID3D11InputLayout* m_pLayout = nullptr;
    //頂点バッファ
    ID3D11Buffer* m_pVertexBuffer = nullptr;
    // 定数バッファ
    ID3D11Buffer* m_pConstantBuffers = nullptr;
    // 頂点シェーダー
    ID3D11VertexShader* m_pVertexShader[2] = { nullptr };
    // ピクセルシェーダー
    ID3D11PixelShader* m_pPixelShader[2] = { nullptr };
    // サンプラーステート
    ID3D11SamplerState* m_pSamplerState=nullptr;

    // レンダーターゲット用のリソース
    ID3D11RenderTargetView* m_pRTV=nullptr; // レンダー ターゲット ビュー

    // 縮小バッファの解像度
    UINT m_Width;
    UINT m_Height;

    LPCSTR vs_main_01 = "VSFunc_Pass1";
    LPCSTR vs_main_02 = "VSFunc_Pass2";
    LPCSTR ps_main_01 = "PSFunc_Pass1";
    LPCSTR ps_main_02 = "PSFunc_Pass2";
public:
    GAUSSIANFILTER();
    ~GAUSSIANFILTER();

    void RemoveDevice();
    HRESULT CompileShaderFromFile(const WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);
    // 初期化
    HRESULT Init(ID3D11Device* pD3DDevice, ID3D11DeviceContext* pD3DDeviceContext, TCHAR pSrcFile[], UINT Width, UINT Height);
    // 定数バッファを設定する
    HRESULT SetConstantBuffers(ID3D11DeviceContext* pD3DDeviceContext,
        float Dispersion);
    void ComputeGaussWeights(float dispersion);
    // 描画
    HRESULT Render(ID3D11Device* pD3DDevice,
        ID3D11DeviceContext* pD3DDeviceContext,
        IN  ID3D11RenderTargetView* pInRTView,        // ガウスフィルターを適応させるレンダーターゲットビュー
        OUT ID3D11RenderTargetView* pOutRTView);     // ガウスフィルターを適応させたレンダーターゲットビュー
};