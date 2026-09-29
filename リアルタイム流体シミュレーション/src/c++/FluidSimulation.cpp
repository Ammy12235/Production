#include "FluidSimulation.h"
#include "ShaderUtility.h"
#include "Mouse.h"
#include "Fps.h"
void FluidSimulation::init(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int width, int height)
{
	_width = width;
	_height = height;
	_device = device;
	_deviceContext = deviceContext;

	HRESULT hr = E_FAIL;
	D3D11_TEXTURE2D_DESC texDesc = {};
	texDesc.Width = width;
	texDesc.Height = height;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	texDesc.SampleDesc.Count = 1;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;//読み書きと読み取り専用どちらも可能にする

	hr = device->CreateTexture2D(&texDesc, nullptr, &p_Velocity[0].p_Texture);
	hr = device->CreateTexture2D(&texDesc, nullptr, &p_Velocity[1].p_Texture);

	hr = device->CreateTexture2D(&texDesc, nullptr, &p_Pressure[0].p_Texture);
	hr = device->CreateTexture2D(&texDesc, nullptr, &p_Pressure[1].p_Texture);

	hr = device->CreateTexture2D(&texDesc, nullptr, &p_Divergence.p_Texture);

	hr = device->CreateTexture2D(&texDesc, nullptr, &p_Preview[0].p_Texture);
	hr = device->CreateTexture2D(&texDesc, nullptr, &p_Preview[1].p_Texture);

	//UAVの設定
	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
	uavDesc.Format = texDesc.Format;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
	uavDesc.Texture2D.MipSlice = 0;

	hr = device->CreateUnorderedAccessView(p_Velocity[0].p_Texture, &uavDesc, &p_Velocity[0].p_UAV);
	hr = device->CreateUnorderedAccessView(p_Velocity[1].p_Texture, &uavDesc, &p_Velocity[1].p_UAV);

	hr = device->CreateUnorderedAccessView(p_Pressure[0].p_Texture, &uavDesc, &p_Pressure[0].p_UAV);
	hr = device->CreateUnorderedAccessView(p_Pressure[1].p_Texture, &uavDesc, &p_Pressure[1].p_UAV);

	hr = device->CreateUnorderedAccessView(p_Divergence.p_Texture, &uavDesc, &p_Divergence.p_UAV);

	hr = device->CreateUnorderedAccessView(p_Preview[0].p_Texture, &uavDesc, &p_Preview[0].p_UAV);
	hr = device->CreateUnorderedAccessView(p_Preview[1].p_Texture, &uavDesc, &p_Preview[1].p_UAV);

	//SRVの設定
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	memset(&srvDesc, 0, sizeof(srvDesc));
	srvDesc.Format = texDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	hr = device->CreateShaderResourceView(p_Velocity[0].p_Texture, &srvDesc, &p_Velocity[0].p_SRV);
	hr = device->CreateShaderResourceView(p_Velocity[1].p_Texture, &srvDesc, &p_Velocity[1].p_SRV);

	hr = device->CreateShaderResourceView(p_Pressure[0].p_Texture, &srvDesc, &p_Pressure[0].p_SRV);
	hr = device->CreateShaderResourceView(p_Pressure[1].p_Texture, &srvDesc, &p_Pressure[1].p_SRV);

	hr = device->CreateShaderResourceView(p_Divergence.p_Texture, &srvDesc, &p_Divergence.p_SRV);

	hr = device->CreateShaderResourceView(p_Preview[0].p_Texture, &srvDesc, &p_Preview[0].p_SRV);
	hr = device->CreateShaderResourceView(p_Preview[1].p_Texture, &srvDesc, &p_Preview[1].p_SRV);

	//シェーダの生成
	ShaderDesc shaderDesc;
	shaderDesc.fileName = "FluidSimulation.hlsl";
	shaderDesc.entryPointName = "UpdateAdvection";
	shaderDesc.shaderModel = "cs_5_0";
	SetShaderDirectory();
	SHADER_FAC.CreateComputeShader(eGenerateType::FROM_FILE, shaderDesc, &p_UpdateAdvection);

	shaderDesc.entryPointName = "UpdateDivergence";
	SHADER_FAC.CreateComputeShader(eGenerateType::FROM_FILE, shaderDesc, &p_UpdateDivergence);
	shaderDesc.entryPointName = "InteractionForce";
	SHADER_FAC.CreateComputeShader(eGenerateType::FROM_FILE, shaderDesc, &p_AddInteractionForce);
	shaderDesc.entryPointName = "UpdatePressure";
	SHADER_FAC.CreateComputeShader(eGenerateType::FROM_FILE, shaderDesc, &p_UpdatePressure);
	shaderDesc.entryPointName = "UpdateVelocity";
	SHADER_FAC.CreateComputeShader(eGenerateType::FROM_FILE, shaderDesc, &p_UpdateVelocity);
	shaderDesc.entryPointName = "UpdateTexture";
	SHADER_FAC.CreateComputeShader(eGenerateType::FROM_FILE, shaderDesc, &p_UpdateTexture);

	//定数バッファを作成
	D3D11_BUFFER_DESC buffer_desc;
	buffer_desc.ByteWidth = sizeof(CBUFFER);
	buffer_desc.Usage = D3D11_USAGE_DYNAMIC;
	buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	buffer_desc.MiscFlags = 0;
	buffer_desc.StructureByteStride = sizeof(CBUFFER);

	device->CreateBuffer(&buffer_desc, nullptr, &p_ConstantBuffer);

	buffer_desc.ByteWidth = sizeof(CBUFFER_EF);
	buffer_desc.Usage = D3D11_USAGE_DYNAMIC;
	buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	buffer_desc.MiscFlags = 0;
	buffer_desc.StructureByteStride = sizeof(CBUFFER_EF);

	device->CreateBuffer(&buffer_desc, nullptr, &p_ConstantBufferEF);


	// サンプラーステートの設定
	D3D11_SAMPLER_DESC samplerDesc;

	//リニア補間
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.MipLODBias = 0;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = samplerDesc.BorderColor[1] = samplerDesc.BorderColor[2] = samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	hr = device->CreateSamplerState(&samplerDesc, &p_SamplerState);

	
	int velValue = 15;
	for (int i = 1; i < 15; i++)
	{
		vel[i].x = sin(rand() % 20) * (rand() % velValue);
		vel[i].y = cos(rand() % 20) * (rand() % velValue);
		cbef._externalForce[i].position.x = 50*i+200;
		cbef._externalForce[i].position.y = 100+ rand()%600;
	}

}

void FluidSimulation::setInitVector(ID3D11ShaderResourceView* initVelocity, ID3D11ShaderResourceView* initPressure)
{


}

ID3D11ShaderResourceView* const* FluidSimulation::getResultSRV()
{

	return &p_Preview[index].p_SRV;
}

ID3D11ShaderResourceView* const* FluidSimulation::getVelocitySRV()
{

	return &p_Velocity[index].p_SRV;
}

ID3D11ShaderResourceView* const* FluidSimulation::getDivergenceSRV()
{

	return &p_Divergence.p_SRV;
}

ID3D11ShaderResourceView* const* FluidSimulation::getPressureSRV()
{

	return &p_Pressure[index].p_SRV;
}
float timerss = 0.0f;

void FluidSimulation::setConstantBuffer()
{
	timerss += 0.05;
	D3D11_MAPPED_SUBRESOURCE pData;
	//定数バッファにデータを書き込む
	if (SUCCEEDED(_deviceContext->Map(p_ConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &pData)))
	{
		cb._Width = _width;
		cb._Height = _height;
		cb._DeltaTime = FPS.GetFrameTime() / 2000;
		cb._Attenuation = 0.99f;
		cb._Scale = 1;
		memcpy_s(pData.pData, pData.RowPitch, (void*)(&cb), sizeof(cb));
		_deviceContext->Unmap(p_ConstantBuffer, 0);
	}

	if (SUCCEEDED(_deviceContext->Map(p_ConstantBufferEF, 0, D3D11_MAP_WRITE_DISCARD, 0, &pData)))
	{
		POINT p;
		Mouse::GetInstance()->GetMouseCursorPos(&p);
		XMFLOAT2 mouseVel;
		Mouse::GetInstance()->GetMouseVelocityX(&mouseVel.x);
		Mouse::GetInstance()->GetMouseVelocityY(&mouseVel.y);
		cbef._externalForce[0].position.x = p.x;
		cbef._externalForce[0].position.y = p.y;
		cbef._externalForce[0].velocity.x = mouseVel.x * 30;
		cbef._externalForce[0].velocity.y = mouseVel.y * 30;

		int value = 5;
		for (int i = 1; i < 15; i++)
		{
			if (cbef._externalForce[i].position.x < 10 || cbef._externalForce[i].position.x > 1000)vel[i].x *= -1;
			if (cbef._externalForce[i].position.y < 10|| cbef._externalForce[i].position.y > 1000)vel[i].y *= -1;
			cbef._externalForce[i].position.x += vel[i].x;
			cbef._externalForce[i].position.y += vel[i].y;
			cbef._externalForce[i].velocity.x = -vel[i].x*value;
			cbef._externalForce[i].velocity.y = -vel[i].y*value;
		}

		memcpy_s(pData.pData, pData.RowPitch, (void*)(&cbef), sizeof(cbef));
		_deviceContext->Unmap(p_ConstantBufferEF, 0);
	}

	//定数バッファをセット
	_deviceContext->CSSetConstantBuffers(0, 1, &p_ConstantBuffer);
	_deviceContext->CSSetConstantBuffers(1, 1, &p_ConstantBufferEF);
}
void FluidSimulation::execute()
{

	_deviceContext->CSSetSamplers(0, 1, &p_SamplerState);
	//=================================
	//　コンピュートシェーダー実行
	//=================================
	//    重要!
	// 同じリソースをUAVとSRVで同時にバインドできないので一工程ずつアンバインドする

	UINT xThread = _width / threadNum;
	UINT yThread = _height / threadNum;
	//====================
	//移流の計算を行う
	//====================
	_deviceContext->CSSetShader(p_UpdateAdvection, nullptr, 0);

	_deviceContext->CSSetShaderResources(0, 1, &p_Velocity[index].p_SRV);

	_deviceContext->CSSetUnorderedAccessViews(0, 1, &p_Velocity[1 - index].p_UAV, nullptr);
	//実行
	_deviceContext->Dispatch(xThread, yThread, 1);

	//入れ替え
	std::swap(p_Velocity[0], p_Velocity[1]);

	// UAV.SRV を明示的にアンバインド(やらないと次の処理で参照できない)
	ID3D11ShaderResourceView* nullSRV = nullptr;
	ID3D11UnorderedAccessView* nullUAV = nullptr;
	_deviceContext->CSSetShaderResources(0, 1, &nullSRV);
	_deviceContext->CSSetUnorderedAccessViews(0, 1, &nullUAV, nullptr);

	//====================
	//外力の計算を行う
	//====================


	_deviceContext->CSSetShader(p_AddInteractionForce, nullptr, 0);

	_deviceContext->CSSetShaderResources(0, 1, &p_Velocity[index].p_SRV);
	_deviceContext->CSSetUnorderedAccessViews(0, 1, &p_Velocity[1 - index].p_UAV, 0);
	//実行
	_deviceContext->Dispatch(xThread, yThread, 1);

	//入れ替え
	std::swap(p_Velocity[0], p_Velocity[1]);
	// UAV を明示的にアンバインド
	_deviceContext->CSSetShaderResources(0, 1, &nullSRV);
	_deviceContext->CSSetUnorderedAccessViews(0, 1, &nullUAV, nullptr);



	//====================
	//発散の計算を行う
	//====================
	_deviceContext->CSSetShader(p_UpdateDivergence, nullptr, 0);

	_deviceContext->CSSetShaderResources(0, 1, &p_Velocity[index].p_SRV);
	_deviceContext->CSSetUnorderedAccessViews(2, 1, &p_Divergence.p_UAV, 0);
	//実行
	_deviceContext->Dispatch(xThread, yThread, 1);
	// UAV を明示的にアンバインド
	_deviceContext->CSSetShaderResources(0, 1, &nullSRV);
	_deviceContext->CSSetUnorderedAccessViews(2, 1, &nullUAV, nullptr);

	//====================
	//圧力の計算を行う
	//====================
	_deviceContext->CSSetUnorderedAccessViews(2, 1, &p_Divergence.p_UAV, 0);
	for (int i = 0; i < 200; i++)
	{

		_deviceContext->CSSetShader(p_UpdatePressure, nullptr, 0);

		_deviceContext->CSSetShaderResources(1, 1, &p_Pressure[index].p_SRV);
		_deviceContext->CSSetUnorderedAccessViews(1, 1, &p_Pressure[1 - index].p_UAV, 0);
		//実行
		_deviceContext->Dispatch(xThread, yThread, 1);
		// UAV を明示的にアンバインド
		//入れ替え
		std::swap(p_Pressure[0], p_Pressure[1]);
		_deviceContext->CSSetShaderResources(1, 1, &nullSRV);
		_deviceContext->CSSetUnorderedAccessViews(1, 1, &nullUAV, nullptr);

	}
	_deviceContext->CSSetUnorderedAccessViews(2, 1, &nullUAV, nullptr);

	//====================
	//速度の計算を行う
	//====================
	_deviceContext->CSSetShader(p_UpdateVelocity, nullptr, 0);

	_deviceContext->CSSetShaderResources(0, 1, &p_Velocity[index].p_SRV);
	_deviceContext->CSSetShaderResources(1, 1, &p_Pressure[1 - index].p_SRV);
	_deviceContext->CSSetUnorderedAccessViews(0, 1, &p_Velocity[1 - index].p_UAV, 0);
	//実行
	_deviceContext->Dispatch(xThread, yThread, 1);
	//入れ替え
	std::swap(p_Velocity[0], p_Velocity[1]);
	// UAV を明示的にアンバインド
	_deviceContext->CSSetShaderResources(0, 1, &nullSRV);
	_deviceContext->CSSetShaderResources(1, 1, &nullSRV);
	_deviceContext->CSSetUnorderedAccessViews(0, 1, &nullUAV, nullptr);


	//====================
	//速度を参照して最終結果をテクスチャに書き込む
	//====================
	_deviceContext->CSSetShader(p_UpdateTexture, nullptr, 0);

	_deviceContext->CSSetShaderResources(0, 1, &p_Velocity[index].p_SRV);
	_deviceContext->CSSetShaderResources(2, 1, &TEX_FAC.getTexture("uv-test.png")->m_srv);
	_deviceContext->CSSetUnorderedAccessViews(3, 1, &p_Preview[1 - index].p_UAV, 0);
	//実行
	_deviceContext->Dispatch(xThread, yThread, 1);

	//入れ替え
	std::swap(p_Preview[0], p_Preview[1]);
	// UAV を明示的にアンバインド
	_deviceContext->CSSetUnorderedAccessViews(0, 1, &nullUAV, nullptr);
	_deviceContext->CSSetUnorderedAccessViews(1, 1, &nullUAV, nullptr);
	_deviceContext->CSSetUnorderedAccessViews(2, 1, &nullUAV, nullptr);
	_deviceContext->CSSetUnorderedAccessViews(3, 1, &nullUAV, nullptr);

}

void FluidSimulation::removeDevice()
{
	//シェーダー開放
	SAFE_RELEASE(p_UpdateAdvection);
	SAFE_RELEASE(p_UpdateDivergence);
	SAFE_RELEASE(p_AddInteractionForce);
	SAFE_RELEASE(p_UpdatePressure);
	SAFE_RELEASE(p_UpdateVelocity);
	SAFE_RELEASE(p_UpdateTexture);

	//テクスチャ解放
	SAFE_RELEASE(p_Velocity[0].p_Texture);
	SAFE_RELEASE(p_Velocity[1].p_Texture);
	SAFE_RELEASE(p_Pressure[0].p_Texture);
	SAFE_RELEASE(p_Pressure[1].p_Texture);
	SAFE_RELEASE(p_Divergence.p_Texture);
	SAFE_RELEASE(p_Preview[0].p_Texture);
	SAFE_RELEASE(p_Preview[1].p_Texture);

	//UAV解放
	SAFE_RELEASE(p_Velocity[0].p_UAV);
	SAFE_RELEASE(p_Velocity[1].p_UAV);
	SAFE_RELEASE(p_Pressure[0].p_UAV);
	SAFE_RELEASE(p_Pressure[1].p_UAV);
	SAFE_RELEASE(p_Divergence.p_UAV);
	SAFE_RELEASE(p_Preview[0].p_UAV);
	SAFE_RELEASE(p_Preview[1].p_UAV);

	//SRV解放
	SAFE_RELEASE(p_Velocity[0].p_SRV);
	SAFE_RELEASE(p_Velocity[1].p_SRV);
	SAFE_RELEASE(p_Pressure[0].p_SRV);
	SAFE_RELEASE(p_Pressure[1].p_SRV);
	SAFE_RELEASE(p_Divergence.p_SRV);
	SAFE_RELEASE(p_Preview[0].p_SRV);
	SAFE_RELEASE(p_Preview[1].p_SRV);

	//バッファ関連解放
	SAFE_RELEASE(p_ConstantBuffer);
	SAFE_RELEASE(p_ConstantBufferEF);

	SAFE_RELEASE(p_SamplerState);



}