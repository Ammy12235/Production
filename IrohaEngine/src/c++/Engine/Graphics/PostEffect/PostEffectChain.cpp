#include "PostEffectChain.h"
#include <IrohaGraphics.h>
#define _DEBUG 1
PostEffectChain::~PostEffectChain()
{
	cleanup();
}

void PostEffectChain::cleanup()
{
	SAFE_RELEASE(_intermediateRTV1);
	SAFE_RELEASE(_intermediateRTV2);
	_effects.clear();
	SAFE_RELEASE(_pVertexShader);
	SAFE_RELEASE(_pPixelShader);
	SAFE_RELEASE(_pLayout);
	SAFE_RELEASE(_pVertexBuffer);
	SAFE_RELEASE(_pConstantBuffer);
	SAFE_RELEASE(_pRTV);
}

bool PostEffectChain::init(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int width, int height)
{
	HRESULT hr = E_FAIL;
	_device = device;
	_deviceContext = deviceContext;

	// インプットレイアウトの定義
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",   0, DXGI_FORMAT_R32G32_FLOAT, 0,  D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },

	};
	UINT numElements = ARRAYSIZE(layout);
#if _DEBUG
	//頂点シェーダーを作成する
	ShaderDesc desc =
	{
		"FinalPass.hlsl",
		 "VS",
		 "vs_5_0"
	};
	hr = SHADER_FAC.CreateVertexShader(eGenerateType::FROM_FILE, desc, &_pVertexShader, layout, numElements, &_pLayout);
	if (FAILED(hr))
		return false;

	//ピクセルシェーダーを作成する
	desc.entryPointName = "PS";
	desc.shaderModel = "ps_5_0";

	hr = SHADER_FAC.CreatePixelShader(eGenerateType::FROM_FILE, desc, &_pPixelShader);
	if (FAILED(hr))
		return false;
#else

	//頂点シェーダーを作成する
	ShaderDesc desc =
	{
		"134",
		 "VS",
		 "vs_5_0"
	};
	hr = SHADER_FAC.CreateVertexShader(eGenerateType::FROM_RESOURCE, desc, &_pVertexShader, layout, numElements, &_pLayout);
	if (FAILED(hr))
		return false;

	//ピクセルシェーダーを作成する
	desc.fileName = "133";
	hr = SHADER_FAC.CreatePixelShader(eGenerateType::FROM_RESOURCE, desc, &_pPixelShader);
	if (FAILED(hr))
		return false;

#endif
	ShaderUtility shaderUtility;

	// 射影座標系上での頂点座標を設定する
	VERTEX v[] = {
			 XMFLOAT3(-1,  -1, 0), XMFLOAT2(0, 1),
			  XMFLOAT3(-1,  1, 0),XMFLOAT2(0, 0),
			  XMFLOAT3(1, -1, 0), XMFLOAT2(1, 1),
			  XMFLOAT3(1, 1, 0),XMFLOAT2(1, 0)
	};

	//頂点バッファを生成する
	shaderUtility.CreateVertexBuffer(_device, &_pVertexBuffer, (void*)v, sizeof(v), D3D11_CPU_ACCESS_WRITE);

	//RTVを生成する
	shaderUtility.CreateRenderTargetView(_device, &_intermediateRTV1, width, height, DXGI_FORMAT_R16G16B16A16_FLOAT);
	shaderUtility.CreateRenderTargetView(_device, &_intermediateRTV2, width, height, DXGI_FORMAT_R16G16B16A16_FLOAT);

	return true;
}
void PostEffectChain::addEffect(PostEffect* postEffect)
{
	_effects.push_back(postEffect);
}

void PostEffectChain::apply(IN ID3D11RenderTargetView* inputRTV, OUT ID3D11RenderTargetView* outputRTV)
{
	ShaderUtility shaderUtility;
	ID3D11RenderTargetView* currentInputRTV = inputRTV;
	ID3D11RenderTargetView* currentOutputRTV = _intermediateRTV1;
	ID3D11ShaderResourceView* pSRV = NULL;

	applyCount = 0;
	ID3D11ShaderResourceView* null = nullptr;
	_deviceContext->PSSetShaderResources(0, 1, &null);

	for (size_t i = 0; i < _effects.size(); i++)//順番に適用していく
	{
		if (_effects[i]->isApply())
		{
			if (applyCount > 0)
			{
				currentInputRTV= currentOutputRTV == _intermediateRTV1 ? _intermediateRTV1 : _intermediateRTV2;//前回の出力を今度は入力にする
				currentOutputRTV = currentOutputRTV == _intermediateRTV1 ? _intermediateRTV2 : _intermediateRTV1;//出力先を入れ替える
			}
			_effects[i]->apply(currentInputRTV, currentOutputRTV);//エフェクトを適用する

			applyCount++;
		}
	}

	//最終結果をバックバッファに出力
	_deviceContext->OMSetRenderTargets(1, &outputRTV, nullptr);
	//描画シェーダを設定
	_deviceContext->VSSetShader(_pVertexShader, NULL, NULL);
	_deviceContext->PSSetShader(_pPixelShader, NULL, NULL);

	pSRV = shaderUtility.GetSRViewFromRTView(_device, currentOutputRTV);
	_deviceContext->PSSetShaderResources(0, 1, &pSRV);

	// インプットレイアウトの設定
	_deviceContext->IASetInputLayout(_pLayout);

	// 頂点バッファ設定
	UINT stride = sizeof(VERTEX);
	UINT offset = 0;
	_deviceContext->IASetVertexBuffers(0, 1, &_pVertexBuffer, &stride, &offset);

	// プリミティブ タイプおよびデータの順序に関する情報を設定
	_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	//描画
	_deviceContext->Draw(4, 0);

	SAFE_RELEASE(pSRV);

	_deviceContext->PSSetShaderResources(0, 1, &null);

}
