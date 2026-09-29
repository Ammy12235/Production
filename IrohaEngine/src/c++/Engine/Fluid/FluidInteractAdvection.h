#pragma once
#include "Core/Base.h"
#include "FluidInteract.h"

enum InteractMode
{
	Mode_Jet,//進行速度と逆向きに発生
	Mode_Manual,//マニュアルで行う
};



class FluidInteractAdvection :public IFluidInteract
{
public:
	FluidInteractAdvection() = default;
	virtual ~FluidInteractAdvection() {}

	void SetVelocity(XMFLOAT2 velocity)//どの方向にどのぐらいの速度を加えるか
	{
		_jetVelocity = velocity;

	}

	XMFLOAT2 GetVelocity()const
	{
		return _jetVelocity;
	}

	void SetMode(InteractMode mode)
	{
		_mode = mode;
	}

	void Update()override
	{
		__super::Update();//今のところ位置の更新しかしない
	}


private:
	InteractMode _mode = Mode_Manual;
	XMFLOAT2 _jetVelocity = { 0,0 };
	bool isActivate = false;
};
