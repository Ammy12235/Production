#pragma once

#include "Fluid.h"

enum FluidInteractPriolity
{
	Primary,
	Secondary,
};

class IFluidInteract :public Fluid//インタラクトに使うモジュールのインターフェースクラス
{
public:

	IFluidInteract() {}
	virtual ~IFluidInteract() {}

	void Update()override;
	void SetPriority(FluidInteractPriolity priority);
	void SetOffset(XMFLOAT2 offset);
	FluidInteractPriolity& GetPriority();
	XMFLOAT2 getPos();
private:
	XMFLOAT2 _pos = { 0,0 };
	XMFLOAT2 _offset;
	FluidInteractPriolity _priority = Primary;
};
