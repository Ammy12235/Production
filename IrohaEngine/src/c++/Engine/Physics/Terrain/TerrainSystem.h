#pragma once
#include "Core/Singleton.h"
#include "Stage.h"

class EngineLoop;

class TerrainSystem :public Singleton<TerrainSystem>
{
public:
	TerrainSystem()=default;
	virtual ~TerrainSystem() = default;
	void AddStage(std::shared_ptr<Stage> stage);
	std::shared_ptr<Stage> GetStage()const;

	//応急処置
	//bool IsHit();
private:
	void ExecuteCollision();
	void RenderTerrain();
	void FinalizeTerrainSystem();
	void CleanupAll();
	void Unregister(UINT32 id);
	std::vector < std::shared_ptr<Stage> > _stages;

	friend class EngineLoop;
public:
};
