#include "DiskBlurFilter.h"
#include "Graphics/Shader/ShaderUtility.h"
#include "Graphics/Shader/ShaderFactory.h"

#define _DEBUG 1

DiskBlurFilter::DiskBlurFilter()
{

}

DiskBlurFilter::~DiskBlurFilter()
{
	cleanup();
}

void DiskBlurFilter::cleanup()
{
	SAFE_RELEASE(m_pVertexBuffer);
	SAFE_RELEASE(m_pRTV);
	SAFE_RELEASE(m_pSamplerState);
	SAFE_RELEASE(m_pConstantBuffer);
	for (int i = 0; i < 2; i++)
		SAFE_RELEASE(m_pPixelShader[i]);
	SAFE_RELEASE(m_pLayout);
	for (int i = 0; i < 2; i++)
		SAFE_RELEASE(m_pVertexShader[i]);
	SAFE_RELEASE(pRTViewDownSample);
}

bool DiskBlurFilter::init(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int width, int height)//初期化
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

	//VS
	ShaderDesc desc =
	{
		"DiskBlurFilter.hlsl",
		 "VS",
		 "vs_5_0"
	};
	hr = SHADER_FAC.CreateVertexShader(eGenerateType::FROM_FILE, desc, &m_pVertexShader[0], layout, numElements, &m_pLayout);
	if (FAILED(hr))
		return false;

	//VSFunc_DS
	desc.entryPointName = "VSFunc_DS";
	desc.shaderModel = "vs_5_0";

	hr = SHADER_FAC.CreateVertexShader(eGenerateType::FROM_FILE, desc, &m_pVertexShader[1], layout, numElements, nullptr);
	if (FAILED(hr))
		return false;

	//ピクセルシェーダーを作成する

	//PS
	desc.entryPointName = "PS";
	desc.shaderModel = "ps_5_0";

	hr = SHADER_FAC.CreatePixelShader(eGenerateType::FROM_FILE, desc, &m_pPixelShader[0]);
	if (FAILED(hr))
		return false;

	//PSFunc_DS
	desc.entryPointName = "PSFunc_DS";
	desc.shaderModel = "ps_5_0";
	hr = SHADER_FAC.CreatePixelShader(eGenerateType::FROM_FILE, desc, &m_pPixelShader[1]);
	if (FAILED(hr))
		return false;
#else

	//頂点シェーダーを作成する

	//VS
	ShaderDesc desc =
	{
		"123",
		 "VS",
		 "vs_5_0"
	};
	hr = SHADER_FAC.CreateVertexShader(eGenerateType::FROM_RESOURCE, desc, &m_pVertexShader[0], layout, numElements, &m_pLayout);
	if (FAILED(hr))
		return false;

	//VSFunc_DS
	desc.fileName = "124";
	hr = SHADER_FAC.CreateVertexShader(eGenerateType::FROM_RESOURCE, desc, &m_pVertexShader[1], layout, numElements, nullptr);
	if (FAILED(hr))
		return false;

	//ピクセルシェーダーを作成する

	//PS
	desc.fileName = "121";
	hr = SHADER_FAC.CreatePixelShader(eGenerateType::FROM_RESOURCE, desc, &m_pPixelShader[0]);
	if (FAILED(hr))
		return false;

	//PSFunc_DS
	desc.fileName = "122";
	hr = SHADER_FAC.CreatePixelShader(eGenerateType::FROM_RESOURCE, desc, &m_pPixelShader[1]);
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

	UINT Width = width / 2;
	UINT Height = height / 2;

	// ダウンサンプリング用の縮小バッファを作成。 1 / 2 サイズ。
	hr = shaderUtility.CreateRenderTargetView(m_pD3DDevice, &pRTViewDownSample, Width, Height, DXGI_FORMAT_R16G16B16A16_FLOAT);
	if (FAILED(hr))
		return false;

	// サンプラーステートの設定
	D3D11_SAMPLER_DESC samplerDesc;

	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.MipLODBias = 0;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = samplerDesc.BorderColor[1] = samplerDesc.BorderColor[2] = samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	hr = m_pD3DDevice->CreateSamplerState(&samplerDesc, &m_pSamplerState);
	if (FAILED(hr)) return false;
	return true;
}

void DiskBlurFilter::apply(IN ID3D11RenderTargetView* inputRTV, OUT ID3D11RenderTargetView* outputRTV)
{
	HRESULT hr = E_FAIL;
	XMFLOAT2 size;

	ID3D11ShaderResourceView* pSRV = NULL;
	ShaderUtility shadeUtility;

	D3D11_VIEWPORT OldViewport;
	D3D11_VIEWPORT NewViewport;


	// Pass0(Step1)を処理(ダウンサンプリング)
	{
		if (scale > 0)
		{
			// ビューポートを退避
			UINT pNumVierports = 1;
			m_pD3DDeviceContext->RSGetViewports(&pNumVierports, &OldViewport);

			// レンダーターゲットビューから縦横のサイズを取得
			size = shadeUtility.GetRTViewSize(pRTViewDownSample);

			// ビューポートのサイズを変更する
			NewViewport.TopLeftX = 0;
			NewViewport.TopLeftY = 0;
			NewViewport.Width = size.x;
			NewViewport.Height = size.y;
			NewViewport.MinDepth = 0.0f;
			NewViewport.MaxDepth = 1.0f;
			m_pD3DDeviceContext->RSSetViewports(1, &NewViewport);
			float ClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
			// 自前のレンダーターゲットビューに切り替え
			m_pD3DDeviceContext->OMSetRenderTargets(1, &pRTViewDownSample, nullptr);
			m_pD3DDeviceContext->ClearRenderTargetView(pRTViewDownSample, ClearColor);
		}
		else
		{
			outputRTV = inputRTV;
			return;
		}
		//描画シェーダを設定
		m_pD3DDeviceContext->VSSetShader(m_pVertexShader[1], NULL, NULL);
		//pD3DDeviceContext->GSSetShader(NULL, NULL, NULL);
		m_pD3DDeviceContext->PSSetShader(m_pPixelShader[1], NULL, NULL);

		pSRV = shadeUtility.GetSRViewFromRTView(m_pD3DDevice, inputRTV);
		// レンダリングテクスチャを設定
		m_pD3DDeviceContext->PSSetShaderResources(0, 1, &pSRV);
		// ピクセルシェーダーにサンプラステートを設定する。
		m_pD3DDeviceContext->PSSetSamplers(0, 1, &m_pSamplerState);


		// 描画

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
	}
	/*
	if (scale <= 0)
	{
		ID3D11ShaderResourceView* null = nullptr;
		m_pD3DDeviceContext->PSSetShaderResources(0, 1, &null);
	}
	*/


	// Pass1を処理(Step2)ディスクブラー
	{
		float ClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
		// 自前のレンダーターゲットビューに切り替え
		m_pD3DDeviceContext->OMSetRenderTargets(1, &outputRTV, nullptr);
		m_pD3DDeviceContext->ClearRenderTargetView(outputRTV, ClearColor);

		//描画シェーダを設定
		m_pD3DDeviceContext->VSSetShader(m_pVertexShader[0], NULL, NULL);
		//pD3DDeviceContext->GSSetShader(NULL, NULL, NULL);
		m_pD3DDeviceContext->PSSetShader(m_pPixelShader[0], NULL, NULL);

		pSRV = shadeUtility.GetSRViewFromRTView(m_pD3DDevice, pRTViewDownSample);

		// シェーダーリソースビューのサイズを取得
		size = shadeUtility.GetSRViewSize(pSRV);
		// 定数バッファを設定
		setConstantBuffers(scale, &size);
		// レンダリングテクスチャを設定
		m_pD3DDeviceContext->PSSetShaderResources(0, 1, &pSRV);
		// ピクセルシェーダーにサンプラステートを設定する。
		m_pD3DDeviceContext->PSSetSamplers(0, 1, &m_pSamplerState);


		// 描画
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

		//ビューポートを元に戻す
		m_pD3DDeviceContext->RSSetViewports(1, &OldViewport);
		// 描画
		m_pD3DDeviceContext->Draw(4, 0);
		SAFE_RELEASE(pSRV);
	}
	ID3D11ShaderResourceView* null = nullptr;
	m_pD3DDeviceContext->PSSetShaderResources(0, 1, &null);

}

HRESULT DiskBlurFilter::setConstantBuffers(float scale, XMFLOAT2* size)
{
	HRESULT hr = E_FAIL;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	if (scale < 0.01f)scale = 0.01f;

	//定数バッファにデータを書き込む
	CBUFFER cb;
	if (SUCCEEDED(m_pD3DDeviceContext->Map(m_pConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
	{
		cb.viewScale.x = size->x;
		cb.viewScale.y = size->y;
		cb.blurScale = scale;
		memcpy_s(mappedResource.pData, mappedResource.RowPitch, (void*)(&cb), sizeof(cb));
		m_pD3DDeviceContext->Unmap(m_pConstantBuffer, 0);
	}

	pre_scale = scale;//前回の結果を残す
	hr = S_OK;
	return hr;
}

// 定数バッファを設定する
void DiskBlurFilter::setScale(float Scale)
{
	if (Scale < 0.0001f)Scale = 0.0001f;
	scale = Scale;
}
