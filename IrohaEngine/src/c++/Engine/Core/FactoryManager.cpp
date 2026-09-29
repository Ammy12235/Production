#include "FactoryManager.h"
#include "Physics/Terrain/TerrainSystem.h"

FactoryManager::FactoryManager()
{
	Init();
}

FactoryManager::~FactoryManager()
{

}

bool FactoryManager::Init()
{
	_factories.push_back(std::make_shared<PlayerFactory>());
	return true;
}

bool FactoryManager::Load(const LevelData& data)
{
	auto stage = std::make_shared<Stage>();//ステージをロード
	stage->Load(data);
	TerrainSystem::GetInstance().AddStage(stage);
	
	for (auto it = _factories.begin(); it != _factories.end();)//各エンティティのロード
	{
		(*it)->Load(data);
		(*it)->Create();
		it++;

	}

	
	return true;
}
