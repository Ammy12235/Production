#include "Atmosphere.h"


void Atmosphere::SetActiveSimulation(bool flag)
{
	isActivate = flag;
	FluidSimulationSystem::GetInstance().IsExecuteFluidSimulation(isActivate);

}

void Atmosphere::Update()
{
	//システムにワールド座標から干渉する
	FluidSimulationSystem::GetInstance().SetWorldPosition(_pos);
	FluidSimulationSystem::GetInstance().SetWorldSize(_size);
}

void Atmosphere::Draw()const
{
	D3D.SetLayer(Layer_0);
	D3D.SetAddBlendDesc(255);
	D3D.DrawSRVTex(FluidSimulationSystem::GetInstance().GetResultSRV(), _pos.x, _pos.y, _size.x, _size.y);
	D3D.SetDefaultBlendDesc(255);
	D3D.SetLayer(Layer_0);

}
