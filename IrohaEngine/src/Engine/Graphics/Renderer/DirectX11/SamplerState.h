#pragma once
#include "Core/Base.h"

//サンプラーステートクラス：描画のサンプリング方法の設定をするクラス。
class SAMPLERSTATE
{
public:
	
	D3D11_SAMPLER_DESC GetDefaultSamplerDesc();
	D3D11_SAMPLER_DESC GetFocusHoleSamplerDesc();

	// サンプラーステートを設定する
	HRESULT SetSamplerState(ID3D11Device* pD3DDevice, ID3D11DeviceContext* pD3DDeviceContext,
		D3D11_SAMPLER_DESC desc);

	
private:
};
