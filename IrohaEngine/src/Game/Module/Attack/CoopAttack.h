#pragma once

#include "IAttack.h"

class CoopAttack :public IAttack
{
public:
	CoopAttack()
	{
		_soundId[0] = SND_FAC.CreateSound("snd/se/attack_hit_0.wav");
		_soundId[1] = SND_FAC.CreateSound("snd/se/attack_hit_1.wav");
		_soundId[2] = SND_FAC.CreateSound("snd/se/attack_hit_2.wav");

		_idInfo._tag = "CoopAttack";
		_collider = AddComponent<CircleCollider>();
		_soundEmitter = AddComponent<SoundEmitter>();
	}
	virtual ~CoopAttack() {};

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
	void SetAttackEntity(Entity& entity)
	{
		_attackEntity.push_back(&entity);
	}

	std::vector<Entity*> GetAttackEntityList()const
	{
		return _attackEntity;
	}

	void SetActive(bool flag)
	{
		_isActive = flag;
		_collider.get()->enabled = flag;
	}
	void SetOffset(XMFLOAT2 offset)
	{
		_collider.get()->setOffset(offset);
	}
	void SetScale(float scale)
	{
		_collider.get()->setRadius(scale);
	}

	void ReactionEnter(Entity& other)override
	{
		IdInfo info = other.GetIdInfo();
		if (info._tag == "Enemy")
		{
			if (_soundEmitter != nullptr)
				_soundEmitter.get()->Play(_soundId[rand() % 3]);
		}
	}
private:

	std::shared_ptr<CircleCollider> _collider = nullptr;
	std::vector<Entity*> _attackEntity;//この攻撃インターフェースで攻撃するエンティティのリスト
	XMFLOAT2 _offset = { 0,0 };
	float _scale = 1.0f;
	bool _isActive = false;
	int _hitSE = 0;

	std::shared_ptr<SoundEmitter> _soundEmitter = nullptr;
	int _soundId[3];
	
};
