#pragma once
#include <Iroha.h>
#include "Enemy/IEnemy.h"
#include "UI/HPBar.h"
#include "Module/SearchCollider.h"

class Electron :public IEnemy
{
private:

	enum State
	{
		State_Idle,
		State_Targeting,
		State_AttackIn,
		State_AttackEnd
	};

	State _state = State_Idle;

	XMFLOAT2 _v = { 0,0 };

	int _texId = 0;

	float _width = (64.0f * 1.0f), _height = (64.0f * 1.0f);

	int _defeatedSound = 0;//倒されたときの音
	int _damagedSound = 0;//ダメージを受けたときの音

	float speed = 1;

	XMFLOAT2 destPos = {};//エレクトロンが目指す場所

	float acceleration = 1;

	float rotateTime = 4;
	float attackInTime = 0.5f;
	float attackSpeed=10;
	float timer = 0.0f;

	std::shared_ptr<CircleCollider> _collider = nullptr;
	std::shared_ptr<FluidInteractHeat> _fluidHeat = nullptr;
	std::shared_ptr<HPBar> _hpBar = nullptr;
	std::shared_ptr<SearchCollider> _searchCollider = nullptr;

	const Entity* targetEntity = nullptr;

	hitInfo _hitInfo;
	bool isDummy = false;
	float phase=0;//三角関数の位相
	float radius=300;

	void ReactionEnter(Entity& other)override;
public:
	Electron();
	Electron(IdInfo idInfo);
	virtual ~Electron();

	void SetTargetEntityAndDummySetUp(const Entity& entity)//ダミー用のセットアップ込みで行う
	{
		targetEntity = &entity;
		isDummy = true;
		phase = Define::PI;
		_state = State_Targeting;
		Destroy(*_searchCollider);
	}

	void Init()override;
	void Update()override;
	void Draw()const override;

	
};
