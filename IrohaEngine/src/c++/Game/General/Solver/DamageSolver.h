#pragma once

#include "Enemy/Common/EnemyParameter.h"
#include "Module/Attack/IAttack.h"


class DamageSolver
{
public:
	DamageSolver() = default;
	virtual ~DamageSolver() = default;

	//最終ダメージを決めてくれる便利関数
	float SolveDamage(const IAttack& attack, Entity& enemy, const EnemyParameter& enemyParameter);
};
