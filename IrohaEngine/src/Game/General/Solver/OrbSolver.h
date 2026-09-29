#pragma once

#include "Enemy/Common/EnemyParameter.h"
#include "Module/Attack/CoopAttack.h"
#include "Module/Attack/IndivAttack.h"
#include "Module/Attack/FinalAttack.h"

//オーブの取得数などを決めてくれる便利関数
class OrbSolver
{
public:
	OrbSolver() = default;
	virtual ~OrbSolver() = default;

	//攻撃に対してダメージからEP、EPからオーブへ変換し、誰に渡すのかというところまでを設定してくれる関数
	void SolveCoopAttack(const CoopAttack& coopAttack,float resultDamage,Entity& enemy,const EnemyParameter& enemyParameter);
	void SolveIndivAttack(const IndivAttack& indivAttack, float resultDamage, Entity& enemy, const EnemyParameter& enemyParameter);
	void SolveFinalAttack(const FinalAttack& finalAttack, float resultDamage,  Entity& enemy,const EnemyParameter& enemyParameter);
};
