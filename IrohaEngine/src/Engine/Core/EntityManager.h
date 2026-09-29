#pragma once

#include "Singleton.h"
#include "Entity.h"
#include "Physics/Terrain/Stage.h"

class EngineLoop;
class Entity;
class Stage;

//エンティティマネージャークラス：すべてのエンティティ（主にコードで書かれた実装など）を管理するクラス。Updateなどはここから一気に呼び出す
class EntityManager :public Singleton<EntityManager>
{
public:
	EntityManager()
	{
		_entities.reserve(10000);

	}
	void AddEntity(std::shared_ptr<Entity> entity);
	void AddStage(std::shared_ptr<Stage> stage);
private:
	void Update();
	void PhysicsUpdate();
	void LateUpdate();
	void Cleanup();
	void CleanupAll();
	void Render()const;

	void FinalizeEntityManager();

	void DrawDebugEntity()const;
	void DrawEntityStatistics();

	std::vector<std::shared_ptr<Entity>> _entities;//エンティティ
	std::vector < std::shared_ptr<Stage> > _stages;//ステージ

	friend class EngineLoop;
};
