#pragma once

#include <Iroha.h>

class FluidSimulationSystem;

class Atmosphere :public Entity
{
public:
	Atmosphere(XMFLOAT2 pos,XMFLOAT2 size):_pos(pos),_size(size)
	{

	};
	virtual ~Atmosphere() = default;

	void SetActiveSimulation(bool flag);
	void Update()override;
	void Draw()const override;
private:
	bool isActivate = false;
	XMFLOAT2 _pos = {0,0};
	XMFLOAT2 _size={ 0,0 };;
};
