#include "Teruteru.h"
#include "General/Solver/PlayerAttackSolver.h"

Teruteru::Teruteru()
{
	_texId = TEX_FAC.CreateTexture("tex/enemy/teruteru.png");
	_idInfo._tag = "Enemy";
	_collider = AddComponent<CircleCollider>();
	_fluidAdvection = AddComponent<FluidInteractAdvection>();

	_hpBar = Instantiate<HPBar>(*this);
}
Teruteru::Teruteru(IdInfo idInfo)
{

	initPos = idInfo._initPosition;
	_idInfo._tag = "Enemy";

	_texId = TEX_FAC.CreateTexture("tex/enemy/teruteru.png");
	_collider = AddComponent<CircleCollider>();
	_fluidAdvection = AddComponent<FluidInteractAdvection>();

	_hpBar = Instantiate<HPBar>(*this);
}

Teruteru::~Teruteru()
{
	
}

void Teruteru::OnDestroy()
{
	Destroy(*_hpBar);
}

void Teruteru::Init()
{
	fallSpeed = 0.01f;
	timer = 0.0f;
	lifeTime = 20;
	_v.x = 5;
	firstUpdate = true;

	enemyParameter.maxHP = 300;
	enemyParameter.hp = 300;
	enemyParameter.attack = 0;
	enemyParameter.touchAttack = 5;
	enemyParameter.defeatedEPFactor = 3.5f;
	enemyParameter.defenseFactor = 0;

	_hpBar->SetOffset(XMFLOAT2(0, -64));
}

void Teruteru::Update()
{
	_fluidAdvection->SetVelocity(XMFLOAT2(-_v.x*0.1f, -_v.y * 0.1f));
	_hpBar->SetHPAndMaxHP(enemyParameter.hp, enemyParameter.maxHP);
	if (firstUpdate)
	{
		initPos = _position;
		firstUpdate = false;
	}
	timer += FPS.GetFrameSecondTime();

	if (initPos.x > _position.x)
	{
		accel = 0.02f;
	}
	else if (initPos.x < _position.x)
	{
		accel = -0.02f;
	}
	_v.x += accel;
	_v.y += fallSpeed;

	if (timer > lifeTime)
	{
		Destroy(*this);
	}

	SetVelocity(_v);
}

void Teruteru::Draw()const
{
	D3D.SetLayer(Layer_0);
	D3D.SetAlignmentBlendDesc(255);
	D3D.DrawRotImage(_texId,_position.x, _position.y, 100, 100, 0, 1);
	D3D.SetDefaultBlendDesc(255);
	D3D.SetLayer(Layer_0);
}

void Teruteru::ReactionEnter(Entity& other)
{
	IdInfo idinfo = other.GetIdInfo();
	if (idinfo._tag == "MainPlayer" || idinfo._tag == "SubPlayer")
	{
		
	}

	PlayerAttackSolver playerAttackSolver;
	playerAttackSolver.SolvePlayerAttack(idinfo._tag, other, *this, enemyParameter);//攻撃時の効果などすべて判定を行う
}
