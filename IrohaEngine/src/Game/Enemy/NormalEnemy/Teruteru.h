#pragma once
#include "Enemy/IEnemy.h"
#include "UI/HPBar.h"
#include "Module/SearchCollider.h"

enum TeruteruType
{
	Type_HorizontalUp,
	Type_HorizontalDown,
	Type_VerticalRight,
	Type_VerticalLeft,
};

class Teruteru :public IEnemy
{
private:

	XMFLOAT2 _v = { 0,0 };

	int _texId = 0;

	float _width = (64.0f * 1.0f), _height = (64.0f * 1.0f);

	int _defeatedSound = 0;//倒されたときの音
	int _damagedSound = 0;//ダメージを受けたときの音

	float flySpeed = 3;

	float lifeTime = 5;
	float lifeTimer = 0.0f;
	float timer = 0.0f;
	XMFLOAT2 initPos;
	float fallSpeed = 0.0f;
	float accel = 0.0f;

	bool firstUpdate = true;
	TeruteruType _type;

	std::shared_ptr<CircleCollider> _collider = nullptr;
	std::shared_ptr<FluidInteractAdvection> _fluidAdvection = nullptr;
	std::shared_ptr<HPBar> _hpBar = nullptr;

	hitInfo _hitInfo;

	void setType(TeruteruType type)
	{
		_type = type;
	}

	void ReactionEnter(Entity& other)override;
public:
	Teruteru();
	Teruteru(IdInfo idInfo);
	virtual ~Teruteru();

	void Init()override;
	void Update()override;
	void Draw()const override;
	void OnDestroy()override;

};
