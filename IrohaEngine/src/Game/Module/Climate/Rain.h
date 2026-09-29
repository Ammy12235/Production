#pragma once

#include <Iroha.h>

class Rain :public Entity
{
private:

	void setRain(int rate);
	float _velocity = 10;
	float _angleFactor = 10;//角度をどれだけばらけさせるか
	float lifeTime = 0.4f;
	int  summonRate=0;//1フレームで何個出すか
	float summonTimer = 0;
	struct RainParameter
	{
		float leftLife = 0.0f;
		float speed = 0.0f;
		XMFLOAT2 pos = {0,0};
	};

	XMFLOAT2 _pos = {0,0};
	std::array<RainParameter, 3000> _rains;

	float dynamicTimer = 0.0f;
	float staticTimer = 0.0f;
	bool isAddDynamic = false;
	bool isAddStatic = false;
public:
	Rain(XMFLOAT2 rainPos) :_pos(rainPos) {};
	virtual ~Rain() = default;

	void Update()override;
	void Draw()const override;
	void SetRainRate(int rate)
	{
		summonRate = rate;
	}
};
