#include "BlendState.h"


// ブレンド ステートを無効にするための設定を取得する
D3D11_RENDER_TARGET_BLEND_DESC BLENDSTATE::GetDefaultBlendDesc()
{
    D3D11_RENDER_TARGET_BLEND_DESC RenderTarget;

    RenderTarget.BlendEnable = TRUE;
    RenderTarget.SrcBlend = D3D11_BLEND_ONE;
    RenderTarget.DestBlend = D3D11_BLEND_ZERO;
    RenderTarget.BlendOp = D3D11_BLEND_OP_ADD;
    RenderTarget.SrcBlendAlpha = D3D11_BLEND_ONE;
    RenderTarget.DestBlendAlpha = D3D11_BLEND_ZERO;
    RenderTarget.BlendOpAlpha = D3D11_BLEND_OP_ADD;
    RenderTarget.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

    return RenderTarget;
}

// 線形合成用ブレンド ステートのための設定を取得する
D3D11_RENDER_TARGET_BLEND_DESC BLENDSTATE::GetAlignmentBlendDesc()
{
    D3D11_RENDER_TARGET_BLEND_DESC RenderTarget;

    RenderTarget.BlendEnable = TRUE;
    RenderTarget.SrcBlend = D3D11_BLEND_SRC_ALPHA;
    RenderTarget.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    RenderTarget.BlendOp = D3D11_BLEND_OP_ADD;
    RenderTarget.SrcBlendAlpha = D3D11_BLEND_ONE;
    RenderTarget.DestBlendAlpha = D3D11_BLEND_ZERO;
    RenderTarget.BlendOpAlpha = D3D11_BLEND_OP_ADD;
    RenderTarget.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

    return RenderTarget;
}

// 加算合成用ブレンド ステートのための設定を取得する
D3D11_RENDER_TARGET_BLEND_DESC BLENDSTATE::GetAddBlendDesc()
{
    D3D11_RENDER_TARGET_BLEND_DESC RenderTarget;

    RenderTarget.BlendEnable = TRUE;
    RenderTarget.SrcBlend = D3D11_BLEND_SRC_ALPHA;
    RenderTarget.DestBlend = D3D11_BLEND_ONE;
    RenderTarget.BlendOp = D3D11_BLEND_OP_ADD;
    RenderTarget.SrcBlendAlpha = D3D11_BLEND_ONE;
    RenderTarget.DestBlendAlpha = D3D11_BLEND_ZERO;
    RenderTarget.BlendOpAlpha = D3D11_BLEND_OP_ADD;
    RenderTarget.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

    return RenderTarget;
}

// 減算合成用ブレンド ステートのための設定を取得する
D3D11_RENDER_TARGET_BLEND_DESC BLENDSTATE::GetSubtractBlendDesc()
{
    D3D11_RENDER_TARGET_BLEND_DESC RenderTarget;

    RenderTarget.BlendEnable = TRUE;
    RenderTarget.SrcBlend = D3D11_BLEND_SRC_ALPHA;
    RenderTarget.DestBlend = D3D11_BLEND_ONE;
    RenderTarget.BlendOp = D3D11_BLEND_OP_REV_SUBTRACT;
    RenderTarget.SrcBlendAlpha = D3D11_BLEND_ONE;
    RenderTarget.DestBlendAlpha = D3D11_BLEND_ZERO;
    RenderTarget.BlendOpAlpha = D3D11_BLEND_OP_ADD;
    RenderTarget.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

    return RenderTarget;
}

// 積算合成用ブレンド ステートのための設定を取得する
D3D11_RENDER_TARGET_BLEND_DESC BLENDSTATE::GetMultipleBlendDesc()
{
    D3D11_RENDER_TARGET_BLEND_DESC RenderTarget;

    RenderTarget.BlendEnable = TRUE;
    RenderTarget.SrcBlend = D3D11_BLEND_ZERO;
    RenderTarget.DestBlend = D3D11_BLEND_SRC_COLOR;
    RenderTarget.BlendOp = D3D11_BLEND_OP_ADD;
    RenderTarget.SrcBlendAlpha = D3D11_BLEND_ONE;
    RenderTarget.DestBlendAlpha = D3D11_BLEND_ZERO;
    RenderTarget.BlendOpAlpha = D3D11_BLEND_OP_ADD;
    RenderTarget.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

    return RenderTarget;
}

// ブレンドステートを設定する
HRESULT BLENDSTATE::SetBlendState(ID3D11Device* pD3DDevice, ID3D11DeviceContext* pD3DDeviceContext,
    D3D11_RENDER_TARGET_BLEND_DESC BlendStateArray[], UINT NumBlendState, BOOL AlphaToCoverageEnable)
{
    HRESULT hr = E_FAIL;

    float blendFactor[4] = { D3D11_BLEND_ZERO, D3D11_BLEND_ZERO, D3D11_BLEND_ZERO, D3D11_BLEND_ZERO };
    ID3D11BlendState* pBlendState = NULL;

    if (NumBlendState > 8)
        goto EXIT;

    D3D11_BLEND_DESC BlendDesc;
    ::ZeroMemory(&BlendDesc, sizeof(BlendDesc));
    BlendDesc.AlphaToCoverageEnable = AlphaToCoverageEnable;
    // TRUEの場合、マルチレンダーターゲットで各レンダーターゲットのブレンドステートの設定を個別に設定できる
    // FALSEの場合、0番目のみが使用される
    BlendDesc.IndependentBlendEnable = FALSE;

    for (UINT i = 0; i < NumBlendState; i++)
    {
        ::CopyMemory(&BlendDesc.RenderTarget[i], &BlendStateArray[i], sizeof(D3D11_RENDER_TARGET_BLEND_DESC));
    }

    hr = pD3DDevice->CreateBlendState(&BlendDesc, &pBlendState);
    if (FAILED(hr))
        goto EXIT;

    pD3DDeviceContext->OMSetBlendState(pBlendState, blendFactor, 0xffffffff);

    hr = S_OK;
EXIT:
    SAFE_RELEASE(pBlendState);
    return hr;
}