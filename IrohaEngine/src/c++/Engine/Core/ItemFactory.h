#pragma once
#include "EntityFactory.h"


//アイテムファクトリークラス：アイテムを生成するクラス。
class ItemFactory :public EntityFactory
{
public:
	ItemFactory();
	~ItemFactory() {};

	virtual bool Load(const LevelData& data)override;
	bool Create()override;
private:

};
