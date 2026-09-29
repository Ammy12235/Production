#pragma once

#include "Core/Singleton.h"
#include "FluidField.h"
#include "FluidInteractAdvection.h"
#include "FluidInteractHeat.h"

class Fluid;
class FluidField;
class EngineLoop;
class ComponentManager;
static size_t m_nextFluidCompTypeID = 0;//GUID

class FluidSimulationSystem :public Singleton<FluidSimulationSystem>
{
public:

	ID3D11ShaderResourceView* const* GetResultSRV();
	ID3D11ShaderResourceView* const* GetVelocitySRV();
	ID3D11ShaderResourceView* const* GetDivergenceSRV();
	ID3D11ShaderResourceView* const* GetPressureSRV();



	void IsExecuteFluidSimulation(bool flag)//流体シミュレーションを行うかどうか
	{
		isExecute = flag;
	}

	void SetWorldPosition(XMFLOAT2 pos)//左上の座標を決める
	{
		_worldPosition = pos;
	}

	void SetWorldSize(XMFLOAT2 size)//左上から伸びる大きさを決める
	{
		_worldSize = size;
	}
	FluidSimulationSystem()=default;
	virtual ~FluidSimulationSystem() { RemoveDevice(); };


private:

	XMFLOAT2 _worldPosition;//ワールドに置いた時の座標
	XMFLOAT2 _worldSize;//ワールドに置いた時のサイズ
	bool isExecute = false;

	void Init(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int width, int height);
	void RemoveDevice();
	void Update();
	void SetConstantBuffer();
	void ExecuteFluidSimulation();
	void Cleanup();
	void CleanupAll();

	template<typename CompType>
	static const size_t GetID()
	{
		static size_t id = [] {
			return ++m_nextFluidCompTypeID;
			}();
		return id;
	}

	template<typename CompType>
	void Register(std::shared_ptr<CompType> fluidModule)//ComponentManagerでFluidであることは確認済み
	{
		if constexpr (std::is_base_of_v<FluidInteractAdvection, CompType>)//移流項にてインタラクションするコンポーネント
		{
			_fluidAdvection.emplace_back(fluidModule);
		}
		else if constexpr (std::is_base_of_v<FluidInteractHeat, CompType>)//発散項にてインタラクションするコンポーネント
		{
			_fluidHeat.emplace_back(fluidModule);
		}
	}


	friend class ComponentManager;
	friend class EngineLoop;

	static const UINT threadNum = 16;//グループサイズ

	struct GPUTexture//使用するGPUテクスチャ
	{
		ID3D11Texture2D* p_Texture = nullptr;
		ID3D11UnorderedAccessView* p_UAV = nullptr;
		ID3D11ShaderResourceView* p_SRV = nullptr;
	};

	GPUTexture p_Velocity[2];//速度テクスチャ
	GPUTexture p_Pressure[2]; //圧力テクスチャ
	GPUTexture p_Divergence; //発散テクスチャ(非圧縮流体のためこれを圧力に作用させる)
	GPUTexture p_Preview[2]; //最終結果

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
	XMFLOAT2 _size;//テクスチャのサイズ


	typedef struct ALIGN16 _CBUFFER
	{
		float _DeltaTime;
		float _Scale;
		float _Width;
		float _Height;
		float _Attenuation;


	}CBUFFER;

	struct ExternalAdvection
	{
		XMFLOAT2 position;//位置
		XMFLOAT2 velocity;//噴出速度
	};

	struct ExternalHeat
	{
		XMFLOAT2 position;//位置
		float heat;//温度
	};

	typedef struct ALIGN16 _CBUFFER_EF
	{
		ExternalAdvection _externalAdvection[16];
		ExternalHeat _externalHeat[16];

	}CBUFFER_EF;

	std::array<ExternalAdvection, 16> _advectionParameters;
	std::array<ExternalHeat, 16> _heatParameters;

	CBUFFER cb;
	CBUFFER_EF cbef;

	UINT index = 0;
	std::vector<std::shared_ptr<FluidField>> _fluidField;
	std::vector<std::shared_ptr<FluidInteractAdvection>> _fluidAdvection;//指向性のあるインタラクトコンポーネント
	std::vector<std::shared_ptr<FluidInteractHeat>> _fluidHeat;//指向性なしのインタラクトコンポーネント

};
