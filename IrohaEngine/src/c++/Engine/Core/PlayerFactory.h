#pragma once
#include "Core/EntityFactory.h"

//プレイヤーファクトリークラス：プレイヤーを生成するクラス。
class PlayerFactory :public EntityFactory
{
private:

	
public:
	virtual bool Load(const LevelData& data)override;
	bool Create()override;
	PlayerFactory();
	~PlayerFactory() {}

};
