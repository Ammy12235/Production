#pragma once

#include "Enemy/Common/EnemyParameter.h"
#include "Module/Attack/CoopAttack.h"
#include "Module/Attack/IndivAttack.h"
class HitStopSolver
{
public:
	HitStopSolver() = default;
	virtual ~HitStopSolver() = default;

	//必要なヒットストップを決めてくれる便利関数
	void SolveCoopHitStop(const CoopAttack& coopAttack, float resultDamage, const EnemyParameter& enemyParameter);
	void SolveIndivHitStop(const IndivAttack& indivAttack, float resultDamage, const EnemyParameter& enemyParameter);
};
