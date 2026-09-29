#pragma once
#include "PostEffect.h"
class PostEffect;

//ガウシアンブラーフィルタークラス：画面にガウスぼかし効果をかけるクラス。
class GaussianBlurFilter :public PostEffect
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

    float _mWeights[8];
    float _dispersion = 0.0f;
    float _preDispersion = 0.0f;

    ID3D11InputLayout* m_pLayout = nullptr; // 入力レイアウト
    ID3D11Buffer* m_pVertexBuffer = nullptr;  //頂点バッファ
    ID3D11Buffer* m_pConstantBuffer = nullptr; // 定数バッファ
    ID3D11VertexShader* m_pVertexShader[3] = { nullptr }; // 頂点シェーダー
    ID3D11PixelShader* m_pPixelShader[3] = { nullptr };// ピクセルシェーダー
    ID3D11SamplerState* m_pSamplerState = nullptr; // サンプラーステート

    ID3D11Device* m_pD3DDevice = nullptr;
    ID3D11DeviceContext* m_pD3DDeviceContext = nullptr;


    // レンダーターゲット用のリソース
    ID3D11RenderTargetView* pRTViewDownSample = nullptr;// ダウンサンプリング用レンダーターゲットビュー
    ID3D11RenderTargetView* pRTViewGF = nullptr;// GaussianFilterの中間レンダーターゲットビュー

     // 縮小バッファの解像度
    UINT m_Width;
    UINT m_Height;

    void computeGaussWeights(float dispersion);
    HRESULT setConstantBuffers(float shimmerScale, XMFLOAT2* size);
public:
    bool init(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int width, int height)override;//初期化
    void apply(IN ID3D11RenderTargetView* inputRTV, OUT ID3D11RenderTargetView* outputRTV)override;
    void setDispersion(float dispersion);
    void cleanup()override;
    GaussianBlurFilter();
    virtual ~GaussianBlurFilter();
};