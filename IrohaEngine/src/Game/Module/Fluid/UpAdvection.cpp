#include "UpAdvection.h"

UpAdvection::UpAdvection()
{
	_fluidAdvection = AddComponent<FluidInteractAdvection>();

	SetTerminalVelocity(100);
	lifeTime = 5;
}

void UpAdvection::Update()
{
	_fluidAdvection->SetVelocity(-GetVelocity());
	timer += FPS.GetFrameSecondTime();
	XMFLOAT2 vel = GetVelocity();
	vel.x += sin(timer);
	vel.y -= 0.1f;
	if (timer > lifeTime)
	{
		Destroy(*this);
	}
}
