#pragma once

#include "IWeather.h"
#include "Module/Fluid/UpAdvection.h"

class Cloudy :public IWeather
{
public:
	Cloudy();
	virtual ~Cloudy() = default;
	virtual void SetIn()override
	{
		__super::SetIn();

		//大気表現モジュールを生成
		for (int i = 0; i < 7; i++)
		{
			auto module = Instantiate<UpAdvection>();
			int randomValue = rand() % 1600 - 800;
			module->SetPosition(XMFLOAT2(-900 , 1000+ randomValue));//横にランダムに並べる

			randomValue = rand() % 4 - 2;
			module->SetVelocity(XMFLOAT2(20, randomValue));
		}
		_weatherSound->Play(weatherSoundId);
	}

	virtual void SetOut()override
	{
		__super::SetOut();
	}
	void Update()override;
private:
	XMFLOAT3 _destHSV;

};
