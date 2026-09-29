#include "EnemySpawner.h"
#include "Enemy/NormalEnemy/JetMan.h"
#include "Enemy/NormalEnemy/Walker.h"
#include "Enemy/NormalEnemy/Electron.h"
#include "Enemy/NormalEnemy/Teruteru.h"

void EnemySpawner::Update()
{


}

void EnemySpawner::Generate()
{
	if (_type == SpawnerType_Ceiling)//天井にあるスポナーならば
	{
		auto enemy = Instantiate<Teruteru>();
		enemy->SetPosition(_position);

	}
	else if (_type == SpawnerType_SideLeft)
	{
		auto enemy = Instantiate<JetMan>();
		enemy->SetPosition(_position);
	}
	else if (_type == SpawnerType_SideRight)
	{
		auto enemy = Instantiate<JetMan>();
		enemy->SetPosition(_position);
	}
	else if (_type == SpawnerType_Middle)
	{
		auto enemy = Instantiate<Walker>();
		enemy->SetPosition(_position);

		auto enemy1 = Instantiate<Teruteru>();
		enemy1->SetPosition(_position);
	}

}
