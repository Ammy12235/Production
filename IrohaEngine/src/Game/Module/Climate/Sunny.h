#pragma once

#include "IWeather.h"
#include "Module/Fluid/UpAdvection.h"

class Sunny :public IWeather
{
public:
	Sunny();
	virtual ~Sunny() = default;
	virtual void SetIn()override
	{
		__super::SetIn();
		//大気表現モジュールを生成
		for (int i = 0; i < 7; i++)
		{
			auto module = Instantiate<UpAdvection>();
			int randomValue = rand() % 1600-800;
			module->SetPosition(XMFLOAT2(800+ randomValue, 2000));//横にランダムに並べる

			randomValue = rand() % 4 - 2;
			module->SetVelocity(XMFLOAT2(randomValue, -20));
		}
		_weatherSound->Play(weatherSoundId);
	}

	void DrawgodLay(float alpha);

	virtual void SetOut()override
	{
		__super::SetOut();
	}
	void Update()override;
private:
	XMFLOAT3 _destHSV;
	int _godlayTex = 0;
	XMFLOAT2 godLayParallaxPos0 = { 0,0 };
	XMFLOAT2 godLayParallaxPos1 = {0,0};
	XMFLOAT2 cameraPallaxPos = { 0,0 };
	XMFLOAT2 preCameraPallaxPos = { 0,0 };
	XMFLOAT2 _godlayPivot = {0,0};
};
