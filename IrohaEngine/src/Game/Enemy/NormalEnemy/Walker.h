#pragma once
#include "Enemy/IEnemy.h"
#include "UI/HPBar.h"
class Walker :public IEnemy
{
private:

	XMFLOAT2 _v = {0,0};

	int _texId = 0;
	float _flyingRegist = 1.0f;
	float _width = (64.0f * 1.0f), _height = (64.0f * 1.0f);

	float accelTimer = 0;
	float jumpPower = 0;

	float stateTimer = 0.0f;
	bool isDown = false;
	float downTime = 0.5f;//ダウンする時間
	float downTimer=0.0f;
	float holdUpTime = 0.5f;//物を持ち上げる時間

	bool isDead = false;
	bool isFloatingAir = false;

	int _defeatedSound = 0;//倒されたときの音
	int _damagedSound = 0;//ダメージを受けたときの音

	std::shared_ptr<RectangleCollider> _collider = nullptr;
	std::shared_ptr<HPBar> _hpBar=nullptr;

	enum eInput_State
	{
		Input_None,
		Input_Right,
		Input_Left,
		Input_Idle,
	};

	enum eState
	{
		State_None,
		State_Walk,//歩行
		State_HoldUp,//持ち上げ
		State_Carry,//運搬
		State_Down,//ダウン
	};

	eInput_State _eInpState;
	eState _state;
	hitInfo _hitInfo;

	// マップチップの辺の位置に座標を調整する
	void ReactToMapchip(hitInfo info, bool isHitSlope);
	void ReactionEnter(Entity& other)override;
public:
	Walker();
	Walker(IdInfo idInfo);
	virtual ~Walker();

	void Init()override;
	void Update()override;
	void Draw()const override;
	void OnDestroy()override;

};
