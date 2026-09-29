#pragma once
#include "DirectX.h"

#define ALIGN16 _declspec(align(16)) 

class SHIMMERFILTER
{
private:
    // ブラーフィルターの定数バッファ定義
    typedef struct ALIGN16 _CBUFFER
    {
        float shimmerScale;
    }CBUFFER;

    float mWeights[8];

    // 入力レイアウト
    ID3D11InputLayout* m_pLayout = nullptr;
    //頂点バッファ
    ID3D11Buffer* m_pVertexBuffer = nullptr;
    // 定数バッファ
    ID3D11Buffer* m_pConstantBuffers = nullptr;
    // 頂点シェーダー
    ID3D11VertexShader* m_pVertexShader=  nullptr ;
    // ピクセルシェーダー
    ID3D11PixelShader* m_pPixelShader =  nullptr ;
    // サンプラーステート
    ID3D11SamplerState* m_pSamplerState = nullptr;

    // レンダーターゲット用のリソース
    ID3D11RenderTargetView* m_pRTV = nullptr; // レンダー ターゲット ビュー

    // 縮小バッファの解像度
    UINT m_Width;
    UINT m_Height;

    LPCSTR vs_main_01 = "VS";
    LPCSTR ps_main_01 = "PS";
public:
    SHIMMERFILTER();
    ~SHIMMERFILTER();

    void RemoveDevice();
    HRESULT CompileShaderFromFile(const WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);
    // 初期化
    HRESULT Init(ID3D11Device* pD3DDevice, ID3D11DeviceContext* pD3DDeviceContext, TCHAR pSrcFile[], UINT Width, UINT Height);
    // 定数バッファを設定する
    HRESULT SetConstantBuffers(ID3D11DeviceContext* pD3DDeviceContext,
        float shimmerScale);
    // 描画
    HRESULT Render(ID3D11Device* pD3DDevice,
        ID3D11DeviceContext* pD3DDeviceContext);
};