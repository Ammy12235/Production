#include "JetMan.h"
#include "General/Solver/PlayerAttackSolver.h"
#include "General/InstantSEEmitter.h"

JetMan::JetMan()
{
	_idInfo._tag = "Enemy";
	_collider = AddComponent<CircleCollider>();
	_fluidAdvection = AddComponent<FluidInteractAdvection>();
	_soundEmitter = AddComponent<SoundEmitter>();
	_searchCollider = Instantiate<SearchCollider>();

	_searchCollider->SetTargetName("MainPlayer");//メインプレイヤーを探す
	_searchCollider->SetRadius(2000);//大きめの範囲にする

	_discoverSE = SND_FAC.CreateSound("snd/se/jetMan_discover.wav");
	_launchSE = SND_FAC.CreateSound("snd/se/jetMan_launch.wav");
	_reachSE= SND_FAC.CreateSound("snd/se/jetMan_reached.wav");

	_texId = TEX_FAC.CreateTexture("tex/enemy/jetMan.png");
}
JetMan::JetMan(IdInfo idInfo)
{
	_idInfo._tag = "Enemy";
	_collider = AddComponent<CircleCollider>();
	_fluidAdvection = AddComponent<FluidInteractAdvection>();
	_soundEmitter = AddComponent<SoundEmitter>();

	_searchCollider = Instantiate<SearchCollider>();
	_searchCollider->SetTargetName("MainPlayer");//メインプレイヤーを探す
	_searchCollider->SetRadius(2000);//大きめの範囲にする

	_discoverSE = SND_FAC.CreateSound("snd/se/jetMan_discover.wav");
	_launchSE = SND_FAC.CreateSound("snd/se/jetMan_launch.wav");
	_reachSE = SND_FAC.CreateSound("snd/se/jetMan_reached.wav");
}

JetMan::~JetMan()
{
	Destroy(*_searchCollider);
}
void JetMan::Init()
{
	SetTerminalVelocity(100);

	enemyParameter.maxHP = 0;
	enemyParameter.hp = 0;
	enemyParameter.attack = 0;
	enemyParameter.touchAttack = 40;
	enemyParameter.defeatedEPFactor = 3.0f;
	enemyParameter.defenseFactor = 0;

	fallSpeed = 0.01f;
	_targetDelayTime = 0.5;
	
	_state = State_Idle;
}

void JetMan::Update()
{

	if (_state == State_Idle)
	{
		if (_searchCollider->IsDiscover())//もし見つけたら
		{
			targetEntity = _searchCollider->GetDiscoverEntity();
			_soundEmitter->SetVolume(0.5f);
			_soundEmitter->Play(_discoverSE);
			_state = State_Targeting;
		}
		_v.y += 0.1;

	}
	else if (_state == State_Targeting)
	{
		_fluidAdvection->SetVelocity(XMM::Normalize(targetEntity->GetPosition() - _position)*2);//プレイヤーと逆方向にジェットを出す
		_timer += FPS.GetFrameSecondTime();
		_v.x += cos(_timer);
		_v.y += sin(_timer*2);
		if (_timer > _targetDelayTime)
		{
			_timer = 0.0f;
			_soundEmitter->SetVolume(1.0f);
			_soundEmitter->Play(_launchSE);
			_state = State_Attack;//突撃
		}
	}
	else if (_state == State_Attack)
	{
		_fluidAdvection->SetVelocity(-_v);
		_v = XMM::Normalize(targetEntity->GetPosition() - _position) * flySpeed;
		flySpeed += acceleration;
	}


	lifeTimer += FPS.GetFrameSecondTime();
	if (lifeTimer > lifeTime)
	{
		Destroy(*this);
	}
	_searchCollider->SetPosition(_position);
	SetVelocity(_v);
}

void JetMan::Draw()const
{
	D3D.SetLayer(Layer_0);
	D3D.SetAlignmentBlendDesc(255);
	D3D.DrawRotImage(_texId,_position.x, _position.y, 50, 50, 0, 1.5f);
	D3D.SetDefaultBlendDesc(255);
	D3D.SetLayer(Layer_0);
}

void JetMan::ReactionEnter(Entity& other)
{
	IdInfo idinfo = other.GetIdInfo();
	if (idinfo._tag == "MainPlayer" || idinfo._tag == "SubPlayer")
	{
		Instantiate<InstantSEEmitter>(_reachSE, 5);
		Destroy(*this);
	}

	PlayerAttackSolver playerAttackSolver;
	playerAttackSolver.SolvePlayerAttack(idinfo._tag, other, *this, enemyParameter);//攻撃時の効果などすべて判定を行う

}
