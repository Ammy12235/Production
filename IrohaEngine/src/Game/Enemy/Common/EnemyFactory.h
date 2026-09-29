#pragma once
#include <Iroha.h>


//エネミーファクトリークラス：敵を生成するクラス。
class EnemyFactory :public EntityFactory
{
public:
	EnemyFactory();
	~EnemyFactory(){};

	virtual bool Load(const LevelData& data)override;
	bool Create()override;
private:

};
