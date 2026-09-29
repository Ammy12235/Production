#include "EntityManager.h"


void EntityManager::AddEntity(std::shared_ptr<Entity> entity)
{
	_entities.push_back(entity);
	_entities.back()->Init();
	_entities.back()->InitPhysics();
}

void EntityManager::AddStage(std::shared_ptr<Stage> stage)
{
	_stages.push_back(stage);

}

void EntityManager::FinalizeEntityManager()
{
	_entities.clear();
	_stages.clear();
}

void EntityManager::Update()
{

	//各エンティティの更新
	for (auto& entity : _entities)
	{

		entity->Update();
	}
}

void EntityManager::PhysicsUpdate()
{
	for (auto& entity : _entities)
	{

		entity->UpdatePhysics();
	}
}

void EntityManager::LateUpdate()
{
	for (auto& entity : _entities)
	{
		entity->LateUpdate();
	}
}

void EntityManager::Cleanup()
{
	//並び変えた後に削除する
	_entities.erase(
		std::remove_if(_entities.begin(), _entities.end(),
			[](const std::shared_ptr<Entity>& entity)
			{
				return entity.get()->_isDelete;

			}),
		_entities.end()
	);
}

void EntityManager::CleanupAll()
{
	_entities.clear();
}

void EntityManager::Render()const
{
	for (auto& entity : _entities)
	{
		entity->Draw();
	}

}

void EntityManager::DrawEntityStatistics()
{
	if (Define::Debug)
	{
		WCHAR str[128];

		size_t num = _entities.size();
		swprintf(str, 128, L"EntityNum:%zd", num);
		DWRITE.DrawFormatText(str, 0, 60, 300, 100, D3D.GetColor(255, 255, 255), 1, 1);
	}
}

void EntityManager::DrawDebugEntity()const
{
	for (auto& entity : _entities)
	{
		entity->DrawDebugEntity();
	}
}

