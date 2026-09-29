#pragma once

#include "IWeather.h"
#include "Rain.h"
#include "Module/Fluid/UpAdvection.h"

class Rainy :public IWeather
{
public:
	Rainy();
	virtual ~Rainy() = default;
	virtual void SetIn()override;

	virtual void SetOut()override
	{
		__super::SetOut();
	}
	void Update()override;
private:
	XMFLOAT3 _destHSV;

	float startDistortion =0.0f;
	float destDistortion = 0.04f;

	float startRainRate = 0;
	float destRainRate = 35;

	float dynamicTimer = 0.0f;
	float staticTimer = 0.0f;

	bool isAddStatic = false;
	bool isAddDynamic = false;

	std::shared_ptr<Rain> _rainParticle;
};
