#include "TerrainSystem.h"

void TerrainSystem::ExecuteCollision()
{

}

void TerrainSystem::CleanupAll()
{
	_stages.clear();
}

void TerrainSystem::RenderTerrain()
{
	if (_stages.empty() == false)
	{
		_stages.back()->Draw();

	}
}

void TerrainSystem::FinalizeTerrainSystem()
{

}

void TerrainSystem::AddStage(std::shared_ptr<Stage> stage)
{
	_stages.push_back(stage);
}

std::shared_ptr<Stage> TerrainSystem::GetStage()const
{
	return _stages.back();
}
