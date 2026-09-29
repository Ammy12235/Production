#pragma once
#include <Iroha.h>

class  HPBar:public Entity
{
public:
	HPBar(const Entity& entity): targetEntity(entity) {};
	~HPBar() {};
	void Update()override;
	void SetOffset(XMFLOAT2 offset) { _offset = offset; }
	void SetHPAndMaxHP(float hp, float maxHP) { _hp = hp; _maxHP = maxHP; }
	void Draw()const override;
private:
	XMFLOAT2 _offset = {0,0};
	const Entity& targetEntity;
	float _hp = 0;
	float _maxHP=0;
};
