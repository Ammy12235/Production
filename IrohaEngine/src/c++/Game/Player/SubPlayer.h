#pragma once

#include <Iroha.h>
#include "Module/Attack/IndivAttack.h"
#include "Module/Attack/FinalAttack.h"
#include "Player/Player.h"

class Player;

//プレイヤークラス：プレイヤーの更新、描画を行うクラス。
class SubPlayer :public Entity
{
private:

	XMFLOAT2 _v = {0,0};
	XMFLOAT2 _a = {0,0};
	int _texId = 0;

	bool isFloatingAir = false;
	bool isJump = false;
	bool isFlip = false;

	float _flyingRegist = 1.0f;
	float width = 64;
	float height = 64;
	float defaultMaxSpeed = 5;
	float maxSpeed = 50;
	float attack = 10;
	float defense = 10;
	float accelTimer = 0.0f;
	float jumpPower = 0.0f;

	int iu = 0;

	//======================================
    // プレイヤー追跡用変数
	//======================================
	XMFLOAT2 destPos = {0,0};
	UINT destXPosOffset=40;//横にどれだけ離れたところを目指すか
	UINT limitPlayerDistance = 800;//これ以上の距離離れてしまったのなら、強制的にプレイヤーの場所にワープする
	UINT moveStopDistance = 30;//目的地点にこの距離だけ近づけたら、動くのをやめる
	UINT jumpDistance = 100;//この距離だけ離れたら、ジャンプすることを考える
	//======================================
	// 攻撃時サブの引き寄せ制御に用いる変数
	//======================================
	float springPower = 0.5f;//引き寄せる力

	//======================================
	// パラメータ保存用変数（外部から変更されたメインに送る値を保存する（旧機構））
	//======================================
	UINT storeEP = 0;

	enum eInput_State
	{
		Input_Idle,
		Input_Up,
		Input_Down,
		Input_Right,
		Input_Left
	};

	eInput_State inputState = eInput_State::Input_Idle;
	eInput_State preInputState = eInput_State::Input_Idle;


	hitInfo _hitInfo;//ステージとの衝突情報

	bool accelerateEvaluate();
	//各マップチップに接触した時のリアクション
	void ReactToMapChip(hitInfo info, bool isHitSlope);
	void ReactionEnter(Entity& other)override;

	std::shared_ptr<RectangleCollider> _collider = nullptr;//自分自身の当たり判定
	Player* _player = nullptr;

public:

	SubPlayer();
	virtual ~SubPlayer();
	void Init()override;
	void Update()override;
	void Draw()const override;

	void SetMainPlayer(Player* player);
	void ReceiveHitStopNotify(float duration, float magnitude);//ヒットストップを外部から通知されたことを受け取る関数（メインを保持しているため直接伝える）

	void AddSubEP(UINT ep);

};
