#include "HSVFilter.h"
#include <IrohaGraphics.h>
#define _DEBUG 1

HSVFilter::HSVFilter()
{

}

HSVFilter::~HSVFilter()
{
	cleanup();
}

void HSVFilter::cleanup()
{
	SAFE_RELEASE(m_pRTV);
	SAFE_RELEASE(m_pConstantBuffer);
	SAFE_RELEASE(m_pVertexBuffer);
	SAFE_RELEASE(m_pLayout);
	SAFE_RELEASE(m_pPixelShader);
	SAFE_RELEASE(m_pVertexShader);
}

bool HSVFilter::init(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int width, int height)//初期化
{
	HRESULT hr;
	SetShaderDirectory();

	//デバイス類設定
	m_pD3DDevice = device;
	m_pD3DDeviceContext = deviceContext;

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
		"HSVFilter.hlsl",
		 "VS",
		 "vs_5_0"
	};
	hr = SHADER_FAC.CreateVertexShader(eGenerateType::FROM_FILE, desc, &m_pVertexShader, layout, numElements, &m_pLayout);
	if (FAILED(hr))
		return false;

	//ピクセルシェーダーを作成する
	desc.entryPointName = "PS";
	desc.shaderModel = "ps_5_0";

	hr = SHADER_FAC.CreatePixelShader(eGenerateType::FROM_FILE, desc, &m_pPixelShader);
	if (FAILED(hr))
		return false;
#else

	//頂点シェーダーを作成する
	ShaderDesc desc =
	{
		"132",
		 "VS",
		 "vs_5_0"
	};
	hr = SHADER_FAC.CreateVertexShader(eGenerateType::FROM_RESOURCE, desc, &m_pVertexShader, layout, numElements, &m_pLayout);
	if (FAILED(hr))
		return false;

	//ピクセルシェーダーを作成する
	desc.fileName = "131";
	hr = SHADER_FAC.CreatePixelShader(eGenerateType::FROM_RESOURCE, desc, &m_pPixelShader);
	if (FAILED(hr))
		return false;

#endif


	// 射影座標系上での頂点座標を設定する
	VERTEX v[] = {
			 XMFLOAT3(-1,  -1, 0), XMFLOAT2(0, 1),
			  XMFLOAT3(-1,  1, 0),XMFLOAT2(0, 0),
			  XMFLOAT3(1, -1, 0), XMFLOAT2(1, 1),
			  XMFLOAT3(1, 1, 0),XMFLOAT2(1, 0)
	};

	ShaderUtility shaderUtility;
	//頂点バッファ、定数バッファを生成する
	hr = shaderUtility.CreateVertexBuffer(m_pD3DDevice, &m_pVertexBuffer, (void*)v, sizeof(v), D3D11_CPU_ACCESS_WRITE);
	if (FAILED(hr))
		return false;
	hr = shaderUtility.CreateConstantBuffer(m_pD3DDevice, &m_pConstantBuffer, nullptr, sizeof(CBUFFER), D3D11_CPU_ACCESS_WRITE);
	if (FAILED(hr))
		return false;

	//RTVを生成する
	hr = shaderUtility.CreateRenderTargetView(m_pD3DDevice, &m_pRTV, width, height, DXGI_FORMAT_R16G16B16A16_FLOAT);
	if (FAILED(hr))
		return false;

	return true;
}
void HSVFilter::apply(IN ID3D11RenderTargetView* inputRTV, OUT ID3D11RenderTargetView* outputRTV)
{
	ShaderUtility shaderUtility;
	ID3D11ShaderResourceView* pSRV = NULL;
	m_pD3DDeviceContext->OMSetRenderTargets(1, &outputRTV, nullptr);//出力先を設定する

	float ClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	m_pD3DDeviceContext->ClearRenderTargetView(outputRTV, ClearColor);//出力先のRTVをクリアする

	//描画シェーダを設定
	m_pD3DDeviceContext->VSSetShader(m_pVertexShader, NULL, NULL);
	m_pD3DDeviceContext->PSSetShader(m_pPixelShader, NULL, NULL);

	pSRV = shaderUtility.GetSRViewFromRTView(m_pD3DDevice, inputRTV);
	m_pD3DDeviceContext->PSSetShaderResources(0, 1, &pSRV);//PSにシェーダーリソースを設定する

	// 描画
	HRESULT hr = E_FAIL;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	//定数バッファにデータを書き込む
	CBUFFER cb;
	if (SUCCEEDED(m_pD3DDeviceContext->Map(m_pConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
	{
		cb.hue = _hue;
		cb.saturation = _saturation;
		cb.value = _value;
		memcpy_s(mappedResource.pData, mappedResource.RowPitch, (void*)(&cb), sizeof(cb));
		m_pD3DDeviceContext->Unmap(m_pConstantBuffer, 0);
	}

	// 頂点シェーダーに定数バッファを設定する
	m_pD3DDeviceContext->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);
	// ピクセルシェーダーに定数バッファを設定する
	m_pD3DDeviceContext->PSSetConstantBuffers(0, 1, &m_pConstantBuffer);
	// インプットレイアウトの設定
	m_pD3DDeviceContext->IASetInputLayout(m_pLayout);

	// 頂点バッファ設定
	UINT stride = sizeof(VERTEX);
	UINT offset = 0;
	m_pD3DDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);
	// プリミティブ タイプおよびデータの順序に関する情報を設定
	m_pD3DDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// 描画
	m_pD3DDeviceContext->Draw(4, 0);
	SAFE_RELEASE(pSRV);
	
	ID3D11ShaderResourceView* null[] = { nullptr ,nullptr };
	m_pD3DDeviceContext->PSSetShaderResources(0, 2, null);

}
void HSVFilter::setParameter(float hue, float saturation, float value)
{
	
	if (hue <= 0.0f)hue = 0.0f;
	if (saturation <= 0.0f)saturation = 0.0f;
	if (value <= 0.0f)value = 0.0f;

	_hue = hue;
	_saturation = saturation;
	_value = value;
}
