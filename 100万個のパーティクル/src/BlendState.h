#pragma once
#include "Base.h"
class BLENDSTATE
{
public:
	// ブレンド ステートを無効にするための設定を取得する
	D3D11_RENDER_TARGET_BLEND_DESC GetDefaultBlendDesc();
	// 線形合成用ブレンド ステートのためのを設定を取得する
	D3D11_RENDER_TARGET_BLEND_DESC GetAlignmentBlendDesc();
	// 加算合成用ブレンド ステートのためのを設定を取得する
	D3D11_RENDER_TARGET_BLEND_DESC GetAddBlendDesc();
	// 減算合成用ブレンド ステートのためのを設定を取得する
	D3D11_RENDER_TARGET_BLEND_DESC GetSubtractBlendDesc();
	// 積算合成用ブレンド ステートのためのを設定を取得する
	D3D11_RENDER_TARGET_BLEND_DESC GetMultipleBlendDesc();
	// ブレンドステートを設定する
	HRESULT SetBlendState(ID3D11Device* pD3DDevice, ID3D11DeviceContext* pD3DDeviceContext,
		D3D11_RENDER_TARGET_BLEND_DESC BlendStateArray[], UINT NumBlendState, BOOL AlphaToCoverageEnable);

private:
};