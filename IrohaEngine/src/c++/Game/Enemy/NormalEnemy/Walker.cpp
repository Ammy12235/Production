#include "Walker.h"

#include "Module/Attack/CoopAttack.h"
#include "Module/Attack/IndivAttack.h"
#include "Module/Attack/FinalAttack.h"

#include "Item/EnergyOrb.h"
#include "General/Solver/PlayerAttackSolver.h"
#include "UI/DamagePopUp.h"

#include "Player/Player.h"
#include "Player/SubPlayer.h"

Walker::Walker(IdInfo idInfo)
{
	_idInfo._initPosition = idInfo._initPosition;
	_idInfo._tag = "Enemy";
	_texId = TEX_FAC.CreateTexture("tex/enemy/walker.png");
	_collider = AddComponent<RectangleCollider>();
	_hpBar = Instantiate<HPBar>(*this);
}

Walker::Walker()
{

	_idInfo._tag = "Enemy";
	_texId = TEX_FAC.CreateTexture("tex/enemy/walker.png");
	_collider = AddComponent<RectangleCollider>();
	_hpBar = Instantiate<HPBar>(*this);
}

Walker::~Walker()
{

}

void  Walker::OnDestroy()
{
	Destroy(*_hpBar);
}


void Walker::Init()
{
	_position = _idInfo._initPosition;
	_eInpState = Input_Right;
	_state = State_Walk;

	enemyParameter.maxHP = 120;
	enemyParameter.hp = 120;
	enemyParameter.attack = 10;
	enemyParameter.touchAttack = 5;
	enemyParameter.defeatedEPFactor = 1.5f;
	enemyParameter.defenseFactor = 0;

	_hpBar->SetOffset(XMFLOAT2(0, -64));

	_v = { 0,0 };
}

void Walker::Update()
{
	_hpBar->SetHPAndMaxHP(enemyParameter.hp, enemyParameter.maxHP);


	//接地判定およびスナップを無効にする
	bool isHitSlope = false;
	TerrainSystem::GetInstance().GetStage()->collideStage(*this, _hitInfo, &isFloatingAir, &isHitSlope, _width, _height);
	ReactToMapchip(_hitInfo, isHitSlope);

	if (isDown)
	{
		downTimer += FPS.GetFrameSecondTime();
		if (downTimer > downTime)
		{
			downTimer = 0.0f;
			isDown = false;
		}
	}
	else
	{


		if (_state == State_Walk || _state == State_Carry)
		{
			if (!TerrainSystem::GetInstance().GetStage()->isHit(_position.x + _v.x, _position.y + _height + 32) && !isFloatingAir)
			{
				if (_eInpState == Input_Left)_eInpState = Input_Right;
				else if (_eInpState == Input_Right)_eInpState = Input_Left;
			}
			if (_eInpState == Input_Right)_v.x = 3;
			else if (_eInpState == Input_Left)_v.x = -3;
		}
		else if (_state == State_HoldUp)
		{
			_v.x *= 0.9f;
			stateTimer += 0.016f;
			if (stateTimer > holdUpTime)
			{
				stateTimer = 0.0f;
				_state = State_Carry;
			}
		}
		else if (_state == State_Down)
		{
			_v.x *= 0.95f;
			stateTimer += 0.016f;
			if (stateTimer > downTime)
			{
				stateTimer = 0.0f;
				_state = State_Walk;

			}
		}

	}
	_v.y += Define::Gravity;
	SetVelocity(_v);
}

void Walker::Draw()const
{
	D3D.SetAlignmentBlendDesc(255);
	D3D.DrawRotImage(_texId, _position.x, _position.y, _width, _height, 0, 1);
	D3D.SetDefaultBlendDesc(255);

	//HPバーの描画
	D3D.SetLayer(Layer_0);


}

void Walker::ReactToMapchip(hitInfo info, bool isHitSlope)
{
	if (isHitSlope)
	{
		jumpPower = 0;

		_v.y = 0;
	}
	if (info.isHitLeft)
	{
		_eInpState = Input_Left;

	}
	if (info.isHitRight)
	{
		_eInpState = Input_Right;
	}
	if (info.isHitTop)
	{
		_v.y = 0;
	}
	if (info.isHitBottom)
	{

	}
}

void Walker::ReactionEnter(Entity& other) {

	IdInfo idinfo = other.GetIdInfo();
	XMFLOAT2 pos = other.GetPosition();
	XMFLOAT2 prePos = other.GetPrePosition();
	XMFLOAT2 otherV = other.GetVelocity();

	if (idinfo._tag == "Enemy")
	{
		if (_position.x - pos.x > 0)
		{
			_eInpState = Input_Right;//右に進む
			_position.x += 0.16f;
		}
		else
		{
			_eInpState = Input_Left;//左に進む
			_position.x -= 0.16f;
		}
	}
	else if (idinfo._tag == "MainPlayer" || idinfo._tag == "SubPlayer")
	{

	}

	PlayerAttackSolver playerAttackSolver;
	playerAttackSolver.SolvePlayerAttack(idinfo._tag, other, *this, enemyParameter);//攻撃時の効果などすべて判定を行う


}
