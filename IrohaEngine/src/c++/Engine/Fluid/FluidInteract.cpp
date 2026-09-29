#include "FluidInteract.h"
#include "Core/Entity.h"

void IFluidInteract::Update()
{
	if (_isDelete || enabled)//削除されるか、非アクティブなら更新しない
	{
		_pos = _entity->GetPosition()+_offset;
	}
}
void IFluidInteract::SetPriority(FluidInteractPriolity priority)
{
	_priority = priority;
}

void IFluidInteract::SetOffset(XMFLOAT2 offset)
{
	_offset = offset;
}

FluidInteractPriolity& IFluidInteract::GetPriority()
{
	return _priority;
}

XMFLOAT2 IFluidInteract::getPos()
{
	return _pos;
}
