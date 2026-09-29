#pragma once

struct EnemyParameter
{
	float maxHP = 0;              //最大HP
	float hp = 0;                 //今のHP
	float defeatedEPFactor = 3.0f;//この攻撃で倒されるとき、プレイヤーに与えるEPはgiveEPの何倍になるか
	float attack = 0;             //攻撃力(数値分そのまま体力を減らす)
	float touchAttack = 0;        //接触したときの攻撃力(数値分そのまま体力を減らす)
	float defenseFactor = 0;      //攻撃カット率（例えば20fなら20%攻撃をカットする。）
	EnemyParameter() {};
};
