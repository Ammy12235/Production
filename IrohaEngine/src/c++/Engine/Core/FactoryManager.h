#pragma once
#include "Singleton.h"

#include "StageFactory.h"
#include "PlayerFactory.h"
#include "EntityFactory.h"
#include "ItemFactory.h"

class Entity;

//ファクトリーマネージャークラス：ファクトリークラス群を束ねるクラス。
class FactoryManager :public Singleton<FactoryManager>
{
public:
	FactoryManager();
	~FactoryManager();
	bool Init();
	bool Load(const LevelData& data);
private:
	std::vector<std::shared_ptr<EntityFactory>> _factories;//ファクトリークラスリスト
	std::shared_ptr<StageFactory> _stgFactory;//ステージの生成を担う

};
