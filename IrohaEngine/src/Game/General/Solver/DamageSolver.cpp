#include "DamageSolver.h"

float DamageSolver::SolveDamage(const IAttack& attack, Entity& enemy, const EnemyParameter& enemyParameter)
{
	float rawDamage = attack.GetDamage();//攻撃判定が持つ攻撃力
	float resultDamage= rawDamage * (100 - enemyParameter.defenseFactor)/100;//ここで最終ダメージを算出する
	return resultDamage;
}
