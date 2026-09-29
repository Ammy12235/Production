#include"FluidSimulationSystem.h"
#include "Graphics/Shader/ShaderUtility.h"
#include "Graphics/Shader/ShaderFactory.h"
#include "Core/Input/Mouse.h"
#include "Core/Process/Fps.h"
#include "FluidInteract.h"

void FluidSimulationSystem::Init(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int width, int height)
{
	_width = width;
	_height = height;
	_size = XMFLOAT2(_width, _height);
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

}

ID3D11ShaderResourceView* const* FluidSimulationSystem::GetResultSRV()
{

	return &p_Preview[index].p_SRV;
}

ID3D11ShaderResourceView* const* FluidSimulationSystem::GetVelocitySRV()
{

	return &p_Velocity[index].p_SRV;
}

ID3D11ShaderResourceView* const* FluidSimulationSystem::GetDivergenceSRV()
{

	return &p_Divergence.p_SRV;
}

ID3D11ShaderResourceView* const* FluidSimulationSystem::GetPressureSRV()
{

	return &p_Pressure[index].p_SRV;
}

void FluidSimulationSystem::Update()
{
	//===========================================
	//全てのインタラクトコンポーネントを更新する。
	//===========================================

	//移流項コンポーネント
	if (_fluidAdvection.empty() == false)//コンポーネントが登録されていたら
	{

		for (auto& p : _fluidAdvection)
		{
			p->Update();
		}

		UINT count = 0;
		//移流項コンポーネント
		for (int i = 0; i < _advectionParameters.size(); i++)//初期化
		{
			_advectionParameters[i].position = XMFLOAT2(0, 0);
			_advectionParameters[i].velocity = XMFLOAT2(0, 0);
		}

		for (int i = 0; i < _fluidAdvection.size(); i++)
		{
			if (_fluidAdvection[i]->enabled == true && _fluidAdvection[i]->GetPriority() == FluidInteractPriolity::Primary)//もし、有効化されていて、優先されるなら
			{
				_advectionParameters[i].position = _fluidAdvection[i]->getPos() - _worldPosition;//位置を入力する(相対座標)
				_advectionParameters[i].velocity = _fluidAdvection[i]->GetVelocity();//速度を入力する

				count++;//すでに入力された数を保存する

				if (_advectionParameters.size() == count)//最大数に達したなら、
				{
					break;//ループを抜ける
				}
			}
		}
	}

	//発散項コンポーネント
	if (_fluidHeat.empty() == false)
	{

		for (auto& p : _fluidHeat)
		{
			p->Update();
		}
		UINT count = 0;
		//発散項コンポーネント

		for (int i = 0; i < _heatParameters.size(); i++)//初期化
		{
			_heatParameters[i].position = XMFLOAT2(0, 0);
			_heatParameters[i].heat = 0;
		}
		for (int i = 0; i < _fluidHeat.size(); i++)//１６回まわす
		{
			if (_fluidHeat[i]->enabled == true && _fluidHeat[i]->GetPriority() == FluidInteractPriolity::Primary)//もし、有効化されていて、優先されるなら
			{
				_heatParameters[i].position = _fluidHeat[i]->getPos() - _worldPosition;//位置を入力する(相対座標)
				_heatParameters[i].heat = _fluidHeat[i]->GetHeatValue();//温度を入力する

				count++;//すでに入力された数を保存する

				if (_heatParameters.size() == count)//最大数に達したなら、
				{
					break;//ループを抜ける
				}
			}
		}
	}

	//===========================================
	//保持しているコンポーネントから定数バッファに送る情報を最大値まで配列に入れる。
	//===========================================


	/*
	if (_advectionParameters.size() < _fluidAdvection.size())//登録されているコンポーネント数がスロット数よりも多ければ、
	{
		for (int i = count; i > 0; i--)//入力されていない
		{

		}
	}
	*/

	SetConstantBuffer();
}

void FluidSimulationSystem::Cleanup()
{
	//並び変えた後に削除する
	_fluidAdvection.erase(
		std::remove_if(_fluidAdvection.begin(), _fluidAdvection.end(),
			[](const std::shared_ptr<FluidInteractAdvection>& module)
			{
				return module->_isDelete;

			}
		),
		_fluidAdvection.end()
	);

	
	_fluidHeat.erase(
		std::remove_if(_fluidHeat.begin(), _fluidHeat.end(),
			[](const std::shared_ptr<FluidInteractHeat>& module)
			{
				return module->_isDelete;

			}
		),
		_fluidHeat.end()
	);
}
void FluidSimulationSystem::CleanupAll()
{
	_fluidAdvection.clear();
	_fluidHeat.clear();
}

void FluidSimulationSystem::SetConstantBuffer()
{
	D3D11_MAPPED_SUBRESOURCE pData;
	//定数バッファにデータを書き込む
	if (SUCCEEDED(_deviceContext->Map(p_ConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &pData)))
	{
		cb._Width = _width;
		cb._Height = _height;
		cb._DeltaTime = FPS.GetFrameMiliTime() / 1000;
		cb._Attenuation = 0.99f;
		cb._Scale = _size.x / _worldSize.x;
		memcpy_s(pData.pData, pData.RowPitch, (void*)(&cb), sizeof(cb));
		_deviceContext->Unmap(p_ConstantBuffer, 0);
	}

	if (SUCCEEDED(_deviceContext->Map(p_ConstantBufferEF, 0, D3D11_MAP_WRITE_DISCARD, 0, &pData)))
	{
		for (int i = 0; i < 15; i++)
		{
			//移流項の入力
			cbef._externalAdvection[i].position = _advectionParameters[i].position;
			cbef._externalAdvection[i].velocity = _advectionParameters[i].velocity;

			//発散項の入力
			cbef._externalHeat[i].position = _heatParameters[i].position;
			cbef._externalHeat[i].heat = _heatParameters[i].heat;
		}

		memcpy_s(pData.pData, pData.RowPitch, (void*)(&cbef), sizeof(cbef));
		_deviceContext->Unmap(p_ConstantBufferEF, 0);
	}

	//定数バッファをセット
	_deviceContext->CSSetConstantBuffers(0, 1, &p_ConstantBuffer);
	_deviceContext->CSSetConstantBuffers(1, 1, &p_ConstantBufferEF);
}

void FluidSimulationSystem::ExecuteFluidSimulation()
{
	if (isExecute == false)return;//実行フラグが立っていなかったら処理しない（すごく重い処理のため）

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
	for (int i = 0; i < 50; i++)
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

void FluidSimulationSystem::RemoveDevice()
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
