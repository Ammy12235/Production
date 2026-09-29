#pragma once

#include "FluidInteract.h"

class FluidInteractHeat :public IFluidInteract
{
public:
	FluidInteractHeat() = default;
	virtual ~FluidInteractHeat() = default;

	void SetHeatValue(float heat)//どのぐらいの温度を加えるか
	{
		
		_heat = heat;

	}

	float GetHeatValue()
	{
		return _heat;
	}

	void Update()override
	{
		__super::Update();//今のところ位置の更新しかしない
		
	}


private:
	float _heat = 0.0f;
	
};
