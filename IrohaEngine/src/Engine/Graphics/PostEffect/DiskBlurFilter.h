#pragma once
#include "PostEffect.h"
class PostEffect;

//ディスクブラーフィルタークラス：画面に円形ぼかし効果をかけるクラス。
class DiskBlurFilter :public PostEffect
{
private:
    // ブラーフィルターの定数バッファ定義
    typedef struct ALIGN16 _CBUFFER
    {
        XMFLOAT2 viewScale;
        float blurScale;

    }CBUFFER;

    // 縮小バッファの解像度
    UINT m_Width;
    UINT m_Height;

    //ブラースケール
    float scale = 0.0f;
    float pre_scale = 0.0f;
   
    ID3D11InputLayout* m_pLayout = nullptr; // 入力レイアウト
    ID3D11Buffer* m_pVertexBuffer = nullptr;  //頂点バッファ
    ID3D11Buffer* m_pConstantBuffer = nullptr; // 定数バッファ
    ID3D11VertexShader* m_pVertexShader[2] = { nullptr }; // 頂点シェーダー
    ID3D11PixelShader* m_pPixelShader[2] = { nullptr };// ピクセルシェーダー
    ID3D11SamplerState* m_pSamplerState = nullptr; // サンプラーステート

    ID3D11Device* m_pD3DDevice = nullptr;
    ID3D11DeviceContext* m_pD3DDeviceContext = nullptr;


    // レンダーターゲット用のリソース
    ID3D11RenderTargetView* m_pRTV = nullptr; // レンダー ターゲット ビュー
    ID3D11RenderTargetView* pRTViewDownSample=nullptr;// ダウンサンプリング用レンダーターゲットビュー

    HRESULT setConstantBuffers(float shimmerScale, XMFLOAT2* size);
public:
    bool init(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int width, int height)override;//初期化
    void apply(IN ID3D11RenderTargetView* inputRTV, OUT ID3D11RenderTargetView* outputRTV)override;
    void setScale(float Scale);
    void cleanup()override;
    DiskBlurFilter();
    virtual ~DiskBlurFilter();
};