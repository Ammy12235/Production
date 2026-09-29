#pragma once
#include <Iroha.h>

//誰かが手を話してしまった風船。割るとEPか回復アイテムが出る
class Balloon :public Entity
{
public:
	Balloon() = default;
	virtual ~Balloon() = default;

	void SetInitVelocity(XMFLOAT2 vel)
	{
		SetVelocity(vel);
	}

	void Update()
	{

	}
private:
	float floatingPower = 0.1;//風船なのでだんだん上に行く
};
