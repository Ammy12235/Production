#include "SamplerState.h"

D3D11_SAMPLER_DESC SAMPLERSTATE::GetDefaultSamplerDesc()
{
	// 異方性フィルタリング補間、Wrapモード
	D3D11_SAMPLER_DESC desc = {};
	desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;	// 何もフィルタリングしない
	desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;		// テクスチャアドレッシングモードをWrapに
	desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;		// テクスチャアドレッシングモードをWrapに
	desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;		// テクスチャアドレッシングモードをWrapに
	desc.MipLODBias = 0;
	desc.MaxAnisotropy = 0;
	desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	desc.BorderColor[0] = 0; desc.BorderColor[1] = 0; desc.BorderColor[2] = 0; desc.BorderColor[3] = 0;
	desc.MinLOD = 0;
	desc.MaxLOD = D3D11_FLOAT32_MAX;

	return desc;

}

D3D11_SAMPLER_DESC SAMPLERSTATE::GetFocusHoleSamplerDesc()
{
	// 異方性フィルタリング補間、Wrapモード
	D3D11_SAMPLER_DESC desc = {};
	desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;	// 何もフィルタリングしない
	desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;		// テクスチャアドレッシングモードをWrapに
	desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;		// テクスチャアドレッシングモードをWrapに
	desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;		// テクスチャアドレッシングモードをWrapに
	desc.MipLODBias = 0;
	desc.MaxAnisotropy = 0;
	desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	desc.BorderColor[0] = 0; desc.BorderColor[1] = 0; desc.BorderColor[2] = 0; desc.BorderColor[3] = 1;
	desc.MinLOD = 0;
	desc.MaxLOD = D3D11_FLOAT32_MAX;

	return desc;
}

// サンプラーステートを設定する
HRESULT SAMPLERSTATE::SetSamplerState(ID3D11Device* pD3DDevice, ID3D11DeviceContext* pD3DDeviceContext,
	D3D11_SAMPLER_DESC desc)
{
	HRESULT hr = S_OK;
	// ステートオブジェクト作成
	ComPtr<ID3D11SamplerState> state;
	hr = pD3DDevice->CreateSamplerState(&desc, &state);
	if (FAILED(hr))return hr;
	// 各シェーダーの0番目にセット
	pD3DDeviceContext->VSSetSamplers(0, 1, state.GetAddressOf()); // 頂点シェーダーの0番目にセット
	pD3DDeviceContext->PSSetSamplers(0, 1, state.GetAddressOf()); // ピクセルシェーダーの0番目にセット
	pD3DDeviceContext->GSSetSamplers(0, 1, state.GetAddressOf()); // ジオメトリシェーダーの0番目にセット
	pD3DDeviceContext->CSSetSamplers(0, 1, state.GetAddressOf()); // コンピュートシェーダーの0番目にセット

	return hr;
}