#pragma once
#include <Iroha.h>
#include "Enemy/IEnemy.h"
#include "UI/HPBar.h"
#include "Module/SearchCollider.h"

class JetMan :public IEnemy
{
private:

	enum State
	{
		State_Idle,//滞在
		State_Targeting,//ターゲット
		State_Attack,//攻撃開始
	};

	State _state;

	XMFLOAT2 _v = { 0,0 };

	int _texId = 0;

	float _width = (64.0f * 1.0f), _height = (64.0f * 1.0f);

	int _defeatedSound = 0;//倒されたときの音
	int _damagedSound = 0;//ダメージを受けたときの音

	float flySpeed = 3;
	float _timer = 0.0f;
	float _targetDelayTime = 0;//ターゲット後に狙いを定めて止まっている時間
	float acceleration =1;
	float fallSpeed = 1;
	float lifeTime = 10;
	float lifeTimer = 0.0f;

	int _discoverSE = 0;
	int _reachSE = 0;
	int _launchSE = 0;

	std::shared_ptr<CircleCollider> _collider = nullptr;
	std::shared_ptr<FluidInteractAdvection> _fluidAdvection = nullptr;
	std::shared_ptr<HPBar> _hpBar = nullptr;
	std::shared_ptr<SearchCollider> _searchCollider=nullptr;
	std::shared_ptr<SoundEmitter> _soundEmitter = nullptr;
	const Entity* targetEntity = nullptr;

	hitInfo _hitInfo;

	void ReactionEnter(Entity& other)override;
public:
	JetMan();
	JetMan(IdInfo idInfo);
	virtual ~JetMan();

	void Init()override;
	void Update()override;
	void Draw()const override;

};
