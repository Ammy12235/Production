#include "PlayerAttackSolver.h"
#include "DamageSolver.h"
#include "OrbSolver.h"
#include "HitStopSolver.h"
#include "UI/DamagePopUp.h"
#include "Manager/GameManager.h"

PlayerAttackSolver::PlayerAttackSolver()
{
	_defeatedSE = SND_FAC.CreateSound("snd/se/defeat.wav");
};

//全ての敵において殴られたらどうなるかの共通の事象をここにまとめる（EPやヒットストップ、ダメージなど）
void PlayerAttackSolver::SolvePlayerAttack(std::string tag, Entity& playerAttack,Entity& enemy, EnemyParameter& enemyParameter)
{
	if (tag == "CoopAttack")
	{
		//攻撃判定をダウンキャストして取得
		CoopAttack* attack = dynamic_cast<CoopAttack*>(&playerAttack);
		SolveCoopAttack(*attack, enemy, enemyParameter);
	}
	else if (tag=="IndivAttack")
	{
		//攻撃判定をダウンキャストして取得
		IndivAttack* attack = dynamic_cast<IndivAttack*>(&playerAttack);
		SolveIndivAttack(*attack, enemy, enemyParameter);
	}
	else if (tag == "FinalAttack")
	{
		//攻撃判定をダウンキャストして取得
		FinalAttack* attack = dynamic_cast<FinalAttack*>(&playerAttack);
		SolveFinalAttack(*attack, enemy, enemyParameter);
	}
}

void PlayerAttackSolver::SolveCoopAttack(const CoopAttack& attack, Entity& enemy, EnemyParameter& enemyParameter)
{

	CameraShaker::GetInstance().GenerateShake(Shake_Random, 0.2f, 1.5f);//カメラを揺らす

	DamageSolver damageSolver;
	float resultDamage = damageSolver.SolveDamage(attack, enemy, enemyParameter);//最終ダメージを算出

	OrbSolver orbSolver;
	orbSolver.SolveCoopAttack(attack, resultDamage, enemy, enemyParameter);//ダメージソルバーから受け取った最終ダメージをもとに取得オーブを作る

	HitStopSolver hitStopSolver;
	hitStopSolver.SolveCoopHitStop(attack, resultDamage, enemyParameter);//ヒットストップの時間と程度を算出

	auto damagePopUp = enemy.Instantiate<DamagePopUp>(enemy.GetPosition(), resultDamage);
	damagePopUp->SetOffset(XMFLOAT2(0, -64));

	//死亡するかの判定
	if ((enemyParameter.hp - resultDamage) < 0)//もし、この攻撃で体力が０になるなら
	{
		GameManager::GetInstance().AddTotalEnemyDamage(resultDamage);
		GameManager::GetInstance().AddDefeatedNum();
		//撃破パーティクルを出し、
		attack.Destroy(enemy);//消す
	}
	else
	{
		enemyParameter.hp -= resultDamage;
		GameManager::GetInstance().AddTotalEnemyDamage(resultDamage);
	}

}


void PlayerAttackSolver::SolveIndivAttack(const IndivAttack& attack, Entity& enemy, EnemyParameter& enemyParameter)
{
	CameraShaker::GetInstance().GenerateShake(Shake_Random, 0.2f, 1.5f);//カメラを揺らす

	DamageSolver damageSolver;
	float resultDamage = damageSolver.SolveDamage(attack, enemy, enemyParameter);//最終ダメージを算出

	OrbSolver orbSolver;
	orbSolver.SolveIndivAttack(attack, resultDamage, enemy, enemyParameter);//ダメージソルバーから受け取った最終ダメージをもとに取得オーブを作る

	HitStopSolver hitStopSolver;
	hitStopSolver.SolveIndivHitStop(attack, resultDamage, enemyParameter);//ヒットストップの時間と程度を算出

	auto damagePopUp = enemy.Instantiate<DamagePopUp>(enemy.GetPosition(), resultDamage);
	damagePopUp->SetOffset(XMFLOAT2(0, -64));

	//死亡するかの判定
	if ((enemyParameter.hp - resultDamage) < 0)//もし、この攻撃で体力が０になるなら
	{
		GameManager::GetInstance().AddTotalEnemyDamage(resultDamage);
		GameManager::GetInstance().AddDefeatedNum();
		//撃破パーティクルを出し、
		attack.Destroy(enemy);//消す
	}
	else
	{
		enemyParameter.hp -= resultDamage;
		GameManager::GetInstance().AddTotalEnemyDamage(resultDamage);
	}
	
}

void PlayerAttackSolver::SolveFinalAttack(const FinalAttack& attack, Entity& enemy, EnemyParameter& enemyParameter)
{
	CameraShaker::GetInstance().GenerateShake(Shake_Random, 0.2f, 5.0f);//カメラを揺らす

	DamageSolver damageSolver;
	float resultDamage = damageSolver.SolveDamage(attack, enemy, enemyParameter);//最終ダメージを算出

	OrbSolver orbSolver;
	orbSolver.SolveFinalAttack(attack, resultDamage, enemy, enemyParameter);//ダメージソルバーから受け取った最終ダメージをもとに取得オーブを作る

	auto damagePopUp = enemy.Instantiate<DamagePopUp>(enemy.GetPosition(), resultDamage);
	damagePopUp->SetOffset(XMFLOAT2(0, -64));

	//死亡するかの判定
	if ((enemyParameter.hp - resultDamage) < 0)//もし、この攻撃で体力が０になるなら
	{

		GameManager::GetInstance().AddTotalEnemyDamage(resultDamage);
		GameManager::GetInstance().AddDefeatedNum();
		//撃破パーティクルを出し、
		attack.Destroy(enemy);//消す

	}
	else
	{
		enemyParameter.hp -= resultDamage;
		GameManager::GetInstance().AddTotalEnemyDamage(resultDamage);
	}
}
