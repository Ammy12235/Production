#pragma once

#include "Base.h"

class FluidSimulation
{
private:
	static const UINT threadNum = 16;

	struct GPUTexture
	{
		ID3D11Texture2D* p_Texture = nullptr;
		ID3D11UnorderedAccessView* p_UAV = nullptr;
		ID3D11ShaderResourceView* p_SRV = nullptr;
	};

	GPUTexture p_Velocity[2];//速度テクスチャ
	GPUTexture p_Pressure[2]; //圧力テクスチャ
	GPUTexture p_Divergence ; //発散テクスチャ(非圧縮流体のためこれを圧力に作用させる)
	GPUTexture p_Preview[2] ; //最終結果

	ID3D11ComputeShader* p_UpdateAdvection = nullptr;
	ID3D11ComputeShader* p_UpdateDivergence = nullptr;
	ID3D11ComputeShader* p_AddInteractionForce = nullptr;
	ID3D11ComputeShader* p_UpdatePressure = nullptr;
	ID3D11ComputeShader* p_UpdateVelocity = nullptr;
	ID3D11ComputeShader* p_UpdateTexture = nullptr;

	
	//デバイス
	ID3D11Device* _device = nullptr;
	ID3D11DeviceContext* _deviceContext = nullptr;

	ID3D11Buffer* p_ConstantBuffer;
	ID3D11Buffer* p_ConstantBufferEF;
	ID3D11SamplerState* p_SamplerState = nullptr;
	UINT _width;
	UINT _height;


	typedef struct ALIGN16 _CBUFFER
	{
		float _DeltaTime;
		float _Scale;
		float _Width;
		float _Height;
		float _Attenuation;
		

	}CBUFFER;

	struct ExternalForce
	{
		XMFLOAT2 position;
		XMFLOAT2 velocity; 
	};

	typedef struct ALIGN16 _CBUFFER_EF
	{
		ExternalForce _externalForce[16];

	}CBUFFER_EF;

	UINT index=0;//スワップするテクスチャのインデックス

	CBUFFER cb;
	CBUFFER_EF cbef;

	XMFLOAT2 vel[16];

public:
	void init(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int width, int height);
	void setInitVector(ID3D11ShaderResourceView* initVelocity, ID3D11ShaderResourceView* initPressure);
	void removeDevice();
	void setConstantBuffer();
	void execute();
	ID3D11ShaderResourceView* const* getResultSRV();
	ID3D11ShaderResourceView* const* getVelocitySRV(); 
	ID3D11ShaderResourceView* const* getDivergenceSRV();
	ID3D11ShaderResourceView* const* getPressureSRV();
	FluidSimulation() {}
	virtual ~FluidSimulation() { removeDevice(); }
};