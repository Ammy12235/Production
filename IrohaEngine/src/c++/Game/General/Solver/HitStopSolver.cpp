#include "HitStopSolver.h"
#include "Player/Player.h"
#include "Player/SubPlayer.h"

void HitStopSolver::SolveCoopHitStop(const CoopAttack& coopAttack, float resultDamage, const EnemyParameter& enemyParameter)
{
	//======================================
	// ヒットストップ量の算出
	//======================================
	float hitStopTime = resultDamage / 500;
	float hitStopMagnitude = enemyParameter.defeatedEPFactor * resultDamage / 50;

	//======================================
	// ヒットストップを各プレイヤーに振り分ける
	//======================================
	auto playerList = coopAttack.GetAttackEntityList();
	for (int i = 0; i < playerList.size(); i++)
	{
		if (playerList[i]->GetIdInfo()._tag == "MainPlayer")
		{
			Player* player = dynamic_cast<Player*>(playerList[i]);
			player->ReceiveHitStopNotify(hitStopTime, hitStopMagnitude);
		}
		else if (playerList[i]->GetIdInfo()._tag == "SubPlayer")
		{
			SubPlayer* subPlayer = dynamic_cast<SubPlayer*>(playerList[i]);
			subPlayer->ReceiveHitStopNotify(hitStopTime, hitStopMagnitude);
		}
	}
}

void HitStopSolver::SolveIndivHitStop(const IndivAttack& indivAttack, float resultDamage, const EnemyParameter& enemyParameter)
{
	//======================================
	// ヒットストップ量の算出
	//======================================
	float hitStopTime = resultDamage / 500;
	float hitStopMagnitude = enemyParameter.defeatedEPFactor * resultDamage / 50;

	//======================================
	// ヒットストップを攻撃したプレイヤーに伝える
	//======================================
	auto entity = indivAttack.GetAttackEntity();

	if (entity->GetIdInfo()._tag == "MainPlayer")
	{
		Player* player = dynamic_cast<Player*>(entity);
		player->ReceiveHitStopNotify(hitStopTime, hitStopMagnitude);
	}
	else if (entity->GetIdInfo()._tag == "SubPlayer")
	{
		SubPlayer* subPlayer = dynamic_cast<SubPlayer*>(entity);
		subPlayer->ReceiveHitStopNotify(hitStopTime, hitStopMagnitude);
	}

}
