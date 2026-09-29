#include "SubPlayer.h"
#include "Enemy/IEnemy.h"

SubPlayer::SubPlayer()
{

	_idInfo._tag = "SubPlayer";

	_collider = AddComponent<RectangleCollider>();
	_texId = TEX_FAC.CreateTexture("tex/player/mei_dot.png");
}

SubPlayer::~SubPlayer()
{

}

void SubPlayer::Init()
{

	_a.x = 0.4f;
	_v.x = 0;
	_v.y = 0;
	maxSpeed = 5;
	width = 64;
	height = 64;
}

void SubPlayer::SetMainPlayer(Player* player)
{
	_player = player;
}

void SubPlayer::AddSubEP(UINT ep)
{
	if (_player != nullptr)
	{
		_player->AddSubEP(ep);
	}
}

bool SubPlayer::accelerateEvaluate()//地面では動きやすく止まりやすく、空中では動きにくく止まりにくくする処理
{
	if (inputState != preInputState)
	{
		accelTimer = 0.0f;
	}

	if (isFloatingAir)_flyingRegist = 0.4f;
	else _flyingRegist = 1.0f;
	if (inputState == Input_Right)
	{
		_v.x += _a.x * _flyingRegist;
		if (_v.x > maxSpeed)_v.x = maxSpeed;
	}
	else if (inputState == Input_Left)
	{
		_v.x -= _a.x * _flyingRegist;
		if (_v.x < -maxSpeed)_v.x = -maxSpeed;
	}
	else if (inputState == Input_Idle)
	{
		if (_v.x > 0)
		{
			accelTimer += 0.016f; _v.x -= accelTimer * _flyingRegist;
			if (_v.x < 0.1f)_v.x = 0.0f;
		}
		else if (_v.x < 0)
		{
			accelTimer += 0.016f; _v.x += accelTimer * _flyingRegist;
			if (_v.x > -0.1f)_v.x = 0.0f;
		}
	}

	return true;
}

void SubPlayer::Update()
{

	bool isHitSlope = false;
	TerrainSystem::GetInstance().GetStage()->collideStage(*this, _hitInfo, &isFloatingAir, &isHitSlope, width, height);
	ReactToMapChip(_hitInfo, isHitSlope);

	accelerateEvaluate();

	//==========================================
	// サブがメインを追跡する処理
	//==========================================


	//キー入力の操作

	//右に行くか左に行くか、止まるかを決める

	//常にサブの背後を目指して動く
	if (_player->isFlip == true)//右を向いていれば
	{
		destPos.y = _player->_position.y;
		destPos.x = _player->_position.x - destXPosOffset;//メインの左側を目指す

	}
	else if (_player->isFlip == false)//左を向いていれば
	{
		destPos.y = _player->_position.y;
		destPos.x = _player->_position.x + destXPosOffset;//メインの右側を目指す
	}
	if (XMM::Distance(destPos, _position) < moveStopDistance)//目指す場所と、自分の位置を測り、一定範囲内ならば
	{
		inputState = Input_Idle;//その場に止まる
	}
	else
	{
		if (destPos.x - _position.x > 0)//目標地点が自分よりも右にあるのなら、
		{
			inputState = Input_Right;//右へ移動する
		}
		else//逆ならば、
		{
			inputState = Input_Left;//左へ移動する
		}
	}

	accelerateEvaluate();//入力を考慮して適切に加速される

	//もし、メインがジャンプ状態で、かつ一定距離以上離れているならば、一緒にジャンプする。
	if (_player->isJump && XMM::Distance(_player->_position, _position) > jumpDistance && (!isJump && !isFloatingAir))//ジャンプの入力
	{
		isJump = true;
		isFloatingAir = true;
		jumpPower = _player->defaultJumpPower + _player->jumpPowerFactor;
		_v.y -= jumpPower;

	}

	//もし、協力攻撃またはサブが攻撃するならば
	if (_player->_mainAttackState == _player->MainAttackState_CoopBeforeOccurence ||
		_player->_mainAttackState == _player->MainAttackState_CoopKeep ||
		_player->_mainAttackState == _player->MainAttackState_CoopKeepAir ||
		_player->_subAttackState == _player->SubAttackState_IndivBeforeOccurence ||
		_player->_subAttackState == _player->SubAttackState_IndivKeep
		)
	{
		XMFLOAT2 destPos;
		if (_player->isFlip == true)//右を向いていれば
		{
			destPos.y = _player->_position.y;
			destPos.x = _player->_position.x - destXPosOffset;//メインの左側

		}
		else if (_player->isFlip == false)//左を向いていれば
		{
			destPos.y = _player->_position.y;
			destPos.x = _player->_position.x + destXPosOffset;//メインの右側
		}
		_v += (_player->_position - destPos) * springPower / 100;//引き寄せられる
	}
	else if (_player->_mainAttackState == _player->MainAttackState_CoopBeforeOccurenceAir ||
		_player->_mainAttackState == _player->MainAttackState_CoopKeepAir ||
		_player->_subAttackState == _player->SubAttackState_IndivBeforeOccurenceAir ||
		_player->_subAttackState == _player->SubAttackState_IndivKeepAir)
	{
		_v += (_player->_position - _position) * springPower / 50;//引き寄せられる
	}

	if (XMM::Distance(_player->_position, _position) > limitPlayerDistance)
	{
		if (_player->isFlip == true)//右を向いていれば
		{
			_position.y = _player->_position.y;
			_position.x = _player->_position.x - destXPosOffset;//メインの左側

		}
		else if (_player->isFlip == false)//左を向いていれば
		{
			_position.y = _player->_position.y;
			_position.x = _player->_position.x + destXPosOffset;//メインの右側
		}

		_player->subInvincibleTimer = _player->stunUncontrollableTime + _player->stunInvincibleTime;
		_player->isSubInvincible = true;
		_player->_subState = _player->Sub_Damaged;
	}

	preInputState = inputState;

	if (!isFloatingAir)
	{

		if (inputState == Input_Right)isFlip = true;
		else if (inputState == Input_Left) isFlip = false;
	}


	_v.y += Define::Gravity;
	SetVelocity(_v);

}

// マップチップと接触したときのリアクション
void SubPlayer::ReactToMapChip(hitInfo info, bool isHitSlope)
{
	if (isHitSlope)
	{
		jumpPower = 0;
		isJump = false;
		_v.y = 0;
	}

	if (info.isHitLeft)
	{

	}
	if (info.isHitRight)
	{

	}
	if (info.isHitTop)
	{
		jumpPower = 0.0f;
		isJump = false;
		_v.y = 0;
	}
	if (info.isHitBottom)
	{
		jumpPower = 0.0f;
		_v.y = 0;
	}


}


void SubPlayer::Draw()const
{
	D3D.SetAlignmentBlendDesc(255);
	if (_player->isSubVisible)
	{
		D3D.DrawRotFlipImage(_texId, _position.x, _position.y - 45, width, height + 13, 0, 2, isFlip);
	}
	D3D.SetDefaultBlendDesc(255);
}

//ほかのエンティティと衝突したときのリアクション
void SubPlayer::ReactionEnter(Entity& other) {
	XMFLOAT2 otherVelocity = other.GetVelocity();
	XMFLOAT2 otherPosition = other.GetPosition();
	XMFLOAT2 otherPrePosition = other.GetPrePosition();

	IdInfo info = other.GetIdInfo();
	if (info._tag == "Enemy")//敵の攻撃に接触したら、
	{
		if (_player->isSubInvincible)return;//無敵フラグが立っているならば、攻撃を受けない

		//ダメージくらい状態でないなら、ダメージくらい状態になる。
		if (_player->_subState != _player->Sub_Damaged && _player->_commonState == _player->Common_None)
		{
			IEnemy* enemy = dynamic_cast<IEnemy*>(&other);
			EnemyParameter* param = enemy->GetEnemyParameter();//接触した敵のパラメータをみる
			float damage = param->touchAttack;
			//EPとHPを減らす。

			if ((float)_player->currentHP - damage < 0)//もし、HPが０になる攻撃を受けたら、
			{
				_player->currentHP = 0;//HPを０にする
				_player->_commonState = _player->Common_Death;//HPが0になったので死んでしまった。
				_player->isMainInvincible = true;
				_player->isSubInvincible = true;
				GameManager::GetInstance().SetState(eScoreModeState::ScoreModeState_GameOver);
				return;
			}


			_player->_soundEmitter->Play(_player->_damagedSE);
		
			_player->subInvincibleTimer = _player->stunUncontrollableTime + _player->stunInvincibleTime;
			//_player->_subState = _player->Sub_Damaged;
			_player->currentHP -= damage;//HPは０で止まるかを考慮しない
			_player->isSubInvincible = true;//メインの無敵解除
			_player->_subState = _player->Sub_Damaged;
		}

	}

};

void SubPlayer::ReceiveHitStopNotify(float duration, float magnitude)//ヒットストップを外部から通知されたことを受け取る関数（メインを保持しているため直接伝える）
{
	_player->isSubHitStop = true;            //ヒットストップフラグをオンにし
	_player->subHitStopTime = duration;      //ヒットストップ時間を設定し、
	_player->subHitStopMagnitude = magnitude;//ヒットストップの大きさを設定し、
	_player->subHitStopOffset = { 0 ,0 };     //ヒットストップのオフセットを初期化し、
	_player->subHitStopStoreVelocity = _v;   //ヒットストップ後の速度を保存しておく
}
