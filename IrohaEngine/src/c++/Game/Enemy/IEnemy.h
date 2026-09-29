#pragma once

#include <Iroha.h>
#include "Enemy/Common/EnemyParameter.h"

//敵のパラメータを取得するためのインターフェース
class IEnemy:public Entity
{
public:
	IEnemy()=default;
	virtual ~IEnemy() = default;

	EnemyParameter* GetEnemyParameter()
	{
		return &enemyParameter;
	}

protected:
	EnemyParameter enemyParameter;//体力、攻撃力などの基礎パラメータ
};
