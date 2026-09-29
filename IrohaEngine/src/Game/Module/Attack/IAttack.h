#pragma once

#include <Iroha.h>

//外部のクラスがこのアタックインターフェースから攻撃力を参照する。（込み入ったことになればもう少し増えるかもしれない）
class IAttack:public Entity
{
public:
	void Update()override {};
	void Draw()const override
	{
	}

	void SetDamage(float damage)
	{
		_damage = damage;
	}

	float GetDamage()const
	{
		return _damage;
	}
protected:
	float _damage = 0;
};
