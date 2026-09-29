#include "Electron.h"
#include "General/Solver/PlayerAttackSolver.h"

Electron::Electron()
{
	_idInfo._tag = "Enemy";
	_collider = AddComponent<CircleCollider>();
	_fluidHeat = AddComponent<FluidInteractHeat>();
	_fluidHeat->SetHeatValue(0);

	_searchCollider = Instantiate<SearchCollider>();
	_searchCollider->SetTargetName("MainPlayer");//メインプレイヤーを探す
	_searchCollider->SetRadius(2000);//大きめの範囲にする

}
Electron::Electron(IdInfo idInfo)
{
	_idInfo._tag = "Enemy";
	_collider = AddComponent<CircleCollider>();
	_fluidHeat = AddComponent<FluidInteractHeat>();
	_fluidHeat->SetHeatValue(0);

	_searchCollider = Instantiate<SearchCollider>();
	_searchCollider->SetTargetName("MainPlayer");//メインプレイヤーを探す
	_searchCollider->SetRadius(2000);//大きめの範囲にする
}

Electron::~Electron()
{
	Destroy(*_searchCollider);
}
void Electron::Init()
{
	SetTerminalVelocity(100);
	_fluidHeat->SetHeatValue(0.1f);
	enemyParameter.maxHP = 0;
	enemyParameter.hp = 0;
	enemyParameter.attack = 0;
	enemyParameter.touchAttack = 20;
	enemyParameter.defeatedEPFactor = 2.0f;
	enemyParameter.defenseFactor = 0;

	speed = 6;
}

void Electron::Update()
{
	if (_state == State_Idle)
	{

		if (_searchCollider->IsDiscover())//もしプレイヤーを見つけたら
		{

			targetEntity = _searchCollider->GetDiscoverEntity();
			if (isDummy == false)
			{
				auto dummy = Instantiate<Electron>();//自分と同じ敵を生成する
				dummy->SetTargetEntityAndDummySetUp(*targetEntity);
				Destroy(*_searchCollider);
				isDummy = true;
				_state = State_Targeting;
			}

		}

		if (_searchCollider)
		{
			_searchCollider->SetPosition(_position);
		}
	}
	else if (_state == State_Targeting)
	{
		timer += FPS.GetFrameSecondTime();
		if (timer > rotateTime)
		{
			_state = State_AttackIn;//攻撃開始
			_fluidHeat->SetHeatValue(1);
			timer = 0.0f;
		}
		XMFLOAT2 pos = targetEntity->GetPosition();
		phase += speed * FPS.GetFrameSecondTime();//３なら一秒に半周ぐらい
		destPos.x = pos.x + cos(phase) * radius;
		destPos.y = pos.y + sin(phase) * radius;
		_position = destPos;
	}
	else if (_state == State_AttackIn)
	{
		timer = 0.0f;
		if (timer > attackInTime)
		{
			_state = State_AttackEnd;//攻撃開始
			timer = 0.0f;
		}
		XMFLOAT2 pos = targetEntity->GetPosition();
		phase += speed * FPS.GetFrameSecondTime();//３なら一秒に半周ぐらい
		destPos.x = pos.x + cos(phase) * radius;
		destPos.y = pos.y + sin(phase) * radius;
		_v = destPos - _position;

		radius += cos(Define::PI / 2 * timer / attackInTime);
	}
	else if (_state == State_AttackEnd)
	{
		XMFLOAT2 pos = targetEntity->GetPosition();
		phase += speed * FPS.GetFrameSecondTime();//３なら一秒に半周ぐらい
		destPos.x = pos.x + cos(phase) * radius;
		destPos.y = pos.y + sin(phase) * radius;
		_v = destPos - _position;
		radius -= cos(Define::PI / 2 * timer / attackInTime) * attackSpeed + 1;//下駄を履かせて絶対に０にはなるようにする
	}
	SetVelocity(_v);
}

void Electron::Draw()const
{
	D3D.DrawRotBox(_position.x, _position.y, 50, 50, 0, 1, D3D.GetColor(25, 25, 255));
}

void Electron::ReactionEnter(Entity& other)
{
	IdInfo idinfo = other.GetIdInfo();
	if (idinfo._tag == "MainPlayer" || idinfo._tag == "SubPlayer")
	{
		Destroy(*this);
	}

	PlayerAttackSolver playerAttackSolver;
	playerAttackSolver.SolvePlayerAttack(idinfo._tag, other, *this, enemyParameter);//攻撃時の効果などすべて判定を行う

}
