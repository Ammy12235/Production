#pragma once

#include "IAttack.h"	

class FinalAttack :public IAttack
{
	public:
	FinalAttack()
	{
		_soundId[0] = SND_FAC.CreateSound("snd/se/attack_hit_0.wav");
		_soundId[1] = SND_FAC.CreateSound("snd/se/attack_hit_1.wav");
		_soundId[2] = SND_FAC.CreateSound("snd/se/attack_hit_2.wav");
		_idInfo._tag = "FinalAttack";
		_collider = AddComponent<CircleCollider>();
		_soundEmitter = AddComponent<SoundEmitter>();
	}
	virtual ~FinalAttack() {};
	void Update()override {};
	void Draw()const override
	{
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
		_attackEntity = &entity;
	}

	Entity* GetAttackEntity()const
	{
		return _attackEntity;
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
	Entity* _attackEntity = nullptr;//この攻撃インターフェースで攻撃するエンティティ
	XMFLOAT2 _offset = { 0,0 };
	float _scale = 1.0f;
	bool _isActive = false;

	std::shared_ptr<SoundEmitter> _soundEmitter = nullptr;
	int _soundId[3];
};
