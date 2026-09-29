#pragma once
#include "Module/Attack/CoopAttack.h"
#include "Module/Attack/IndivAttack.h"
#include "Enemy/Common/EnemyParameter.h"
#include "Module/Attack/FinalAttack.h"

class PlayerAttackSolver
{
public:
	PlayerAttackSolver();
	virtual ~PlayerAttackSolver() = default;


	void SolvePlayerAttack(std::string tag,Entity&playerAttack, Entity& enemy, EnemyParameter& enemyParameter);
private:
	void SolveCoopAttack(const CoopAttack& attack, Entity& enemy, EnemyParameter& enemyParameter);
	void SolveIndivAttack(const IndivAttack& attack, Entity& enemy, EnemyParameter& enemyParameter);
	void SolveFinalAttack(const FinalAttack& attack, Entity& enemy, EnemyParameter& enemyParameter);

	int _defeatedSE = 0;
};
