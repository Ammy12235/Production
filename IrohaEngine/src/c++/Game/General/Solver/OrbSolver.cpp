#include "OrbSolver.h"
#include "Item/EnergyOrb.h"

void OrbSolver::SolveCoopAttack(const CoopAttack& coopAttack,float resultDamage, Entity& enemy,const EnemyParameter& enemyParameter)
{
	//====================
	//ダメージ->取得EPへの変換
	//====================
	float ep = 0;//取得EP
	if (enemyParameter.hp - resultDamage < 0)//もしこの攻撃を受けると倒されるのならば
	{
		ep = enemyParameter.defeatedEPFactor * resultDamage / 30;//撃破ボーナスとして何倍かの係数をかけてEPを算出
	}
	else//倒されないなら
	{
		ep = resultDamage / 30;//普通に計算する
	}

	//====================
	//EP->オーブへの変換
	//====================
	UINT divideEP = (UINT)std::floor(ep / 2);//算出したEPを２等分する(小数点以下は切り捨てて、整数にする)
	auto attackList = coopAttack.GetAttackEntityList();//攻撃者のエンティティリストを取得
	//大中小のオーブのうちどれを何個出すかを求める
	UINT lastEP = divideEP;
	UINT bigNum = lastEP / 10;//割った数出る
	lastEP = lastEP % 10;//あまりを残りとして次に回す

	UINT middleNum = lastEP / 5;
	lastEP = lastEP % 5;//あまりを残りとして次に回す

	UINT smallNum = lastEP;//残りは最小オーブである
	UINT count = 0;
	for (auto it : attackList)//２人分回す
	{
		for (int i = 0; i < bigNum; i++)//大オーブを召喚
		{
			EnergyOrb* orb = enemy.Instantiate<EnergyOrb>().get();
			orb->SetPosition(enemy.GetPosition());
			orb->SetTargetPlayer(it, count);
			orb->SetOrbEP(10);
			float angle = rand() % 360;
			float speed = rand() % 100;
			float degToRad = angle / 180 * Define::PI;
			orb->SetInitVelocity(XMFLOAT2(cos(degToRad) * speed, sin(degToRad) * speed));
			orb->SetReachLeftTime(1);

		}

		for (int i = 0; i < middleNum; i++)//中オーブを召喚
		{
			EnergyOrb* orb = enemy.Instantiate<EnergyOrb>().get();
			orb->SetPosition(enemy.GetPosition());
			orb->SetTargetPlayer(it, count);
			orb->SetOrbEP(5);
			float angle = rand() % 360;
			float speed = rand() % 100;
			float degToRad = angle / 180 * Define::PI;
			orb->SetInitVelocity(XMFLOAT2(cos(degToRad) * speed, sin(degToRad) * speed));
			orb->SetReachLeftTime(1);

		}

		for (int i = 0; i < smallNum; i++)//小オーブを召喚
		{
			EnergyOrb* orb = enemy.Instantiate<EnergyOrb>().get();
			orb->SetPosition(enemy.GetPosition());
			orb->SetTargetPlayer(it, count);
			orb->SetOrbEP(1);
			float angle = rand() % 360;
			float speed = rand() % 100;
			float degToRad = angle / 180 * Define::PI;
			orb->SetInitVelocity(XMFLOAT2(cos(degToRad) * speed, sin(degToRad) * speed));
			orb->SetReachLeftTime(1);

		}
		count++;
	}
}

void OrbSolver::SolveIndivAttack(const IndivAttack& indivAttack, float resultDamage,Entity& entity,const EnemyParameter& enemyParameter)
{
	//====================
	//ダメージ->取得EPへの変換
	//====================
	float ep = 0;//取得EP
	if (enemyParameter.hp - resultDamage < 0)//もしこの攻撃を受けると倒されるのならば
	{
		ep = enemyParameter.defeatedEPFactor * resultDamage / 20;//撃破ボーナスとして何倍かの係数をかけてEPを算出
	}
	else//倒されないなら
	{
		ep = resultDamage/20 ;//普通に計算する
	}

	//大中小のオーブのうちどれを何個出すかを求める
	UINT lastEP = ep;
	UINT bigNum = lastEP / 10;//割った数出る
	lastEP = lastEP % 10;//あまりを残りとして次に回す

	UINT middleNum = lastEP / 5;
	lastEP = lastEP % 5;//あまりを残りとして次に回す

	UINT smallNum = lastEP;//残りは最小オーブである


	UINT count = 0;
	auto attackPlayer = indivAttack.GetAttackEntity();//攻撃者を取得
	if (attackPlayer != nullptr)
	{

		if (attackPlayer->GetIdInfo()._tag == "MainPlayer")
		{
			count = 0;
		}
		else if (attackPlayer->GetIdInfo()._tag == "SubPlayer")
		{
			count = 1;
		}
	}



	for (int i = 0; i < bigNum; i++)//大オーブを召喚
	{
		EnergyOrb* orb = entity.Instantiate<EnergyOrb>().get();
		orb->SetPosition(entity.GetPosition());
		orb->SetTargetPlayer(attackPlayer, count);
		orb->SetOrbEP(10);
		float angle = rand() % 360;
		float speed = rand() % 100;
		float degToRad = angle / 180 * Define::PI;
		orb->SetInitVelocity(XMFLOAT2(cos(degToRad) * speed, sin(degToRad) * speed));
		orb->SetReachLeftTime(1);

	}

	for (int i = 0; i < middleNum; i++)//中オーブを召喚
	{
		EnergyOrb* orb = entity.Instantiate<EnergyOrb>().get();
		orb->SetPosition(entity.GetPosition());
		orb->SetTargetPlayer(attackPlayer, count);
		orb->SetOrbEP(5);
		float angle = rand() % 360;
		float speed = rand() % 100;
		float degToRad = angle / 180 * Define::PI;
		orb->SetInitVelocity(XMFLOAT2(cos(degToRad) * speed, sin(degToRad) * speed));
		orb->SetReachLeftTime(1);

	}

	for (int i = 0; i < smallNum; i++)//小オーブを召喚
	{
		EnergyOrb* orb = entity.Instantiate<EnergyOrb>().get();
		orb->SetPosition(entity.GetPosition());
		orb->SetTargetPlayer(attackPlayer, count);
		orb->SetOrbEP(1);
		float angle = rand() % 360;
		float speed = rand() % 100;
		float degToRad = angle / 180 * Define::PI;
		orb->SetInitVelocity(XMFLOAT2(cos(degToRad) * speed, sin(degToRad) * speed));
		orb->SetReachLeftTime(1);

	}
}


void OrbSolver::SolveFinalAttack(const FinalAttack& finalAttack, float resultDamage, Entity& entity, const EnemyParameter& enemyParameter)
{
	//====================
	//ダメージ->取得EPへの変換
	//====================
	float ep = 0;//取得EP
	if (enemyParameter.hp - resultDamage < 0)//もしこの攻撃を受けると倒されるのならば
	{
		ep = enemyParameter.defeatedEPFactor * resultDamage / 30;//撃破ボーナスとして何倍かの係数をかけてEPを算出
	}
	else//倒されないなら
	{
		ep = resultDamage / 30;//普通に計算する
	}

	//大中小のオーブのうちどれを何個出すかを求める
	UINT lastEP = ep;
	UINT bigNum = lastEP / 10;//割った数出る
	lastEP = lastEP % 10;//あまりを残りとして次に回す

	UINT middleNum = lastEP / 5;
	lastEP = lastEP % 5;//あまりを残りとして次に回す

	UINT smallNum = lastEP;//残りは最小オーブである


	UINT count = 0;
	auto attackPlayer = finalAttack.GetAttackEntity();//攻撃者を取得
	if (attackPlayer != nullptr)
	{

		if (attackPlayer->GetIdInfo()._tag == "MainPlayer")
		{
			count = 0;
		}
		else if (attackPlayer->GetIdInfo()._tag == "SubPlayer")
		{
			count = 1;
		}
	}



	for (int i = 0; i < bigNum; i++)//大オーブを召喚
	{
		EnergyOrb* orb = entity.Instantiate<EnergyOrb>().get();
		orb->SetPosition(entity.GetPosition());
		orb->SetTargetPlayer(attackPlayer, count);
		orb->SetOrbEP(10);
		float angle = rand() % 360;
		float speed = rand() % 100;
		float degToRad = angle / 180 * Define::PI;
		orb->SetInitVelocity(XMFLOAT2(cos(degToRad) * speed, sin(degToRad) * speed));
		orb->SetReachLeftTime(1);

	}

	for (int i = 0; i < middleNum; i++)//中オーブを召喚
	{
		EnergyOrb* orb = entity.Instantiate<EnergyOrb>().get();
		orb->SetPosition(entity.GetPosition());
		orb->SetTargetPlayer(attackPlayer, count);
		orb->SetOrbEP(5);
		float angle = rand() % 360;
		float speed = rand() % 100;
		float degToRad = angle / 180 * Define::PI;
		orb->SetInitVelocity(XMFLOAT2(cos(degToRad) * speed, sin(degToRad) * speed));
		orb->SetReachLeftTime(1);

	}

	for (int i = 0; i < smallNum; i++)//小オーブを召喚
	{
		EnergyOrb* orb = entity.Instantiate<EnergyOrb>().get();
		orb->SetPosition(entity.GetPosition());
		orb->SetTargetPlayer(attackPlayer, count);
		orb->SetOrbEP(1);
		float angle = rand() % 360;
		float speed = rand() % 100;
		float degToRad = angle / 180 * Define::PI;
		orb->SetInitVelocity(XMFLOAT2(cos(degToRad) * speed, sin(degToRad) * speed));
		orb->SetReachLeftTime(1);

	}
}
