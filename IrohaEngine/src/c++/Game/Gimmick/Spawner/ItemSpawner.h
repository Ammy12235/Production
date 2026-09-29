#pragma once
#include "ISpawner.h"

class ItemSpawner :public ISpawner
{
public:
	ItemSpawner() = default;
	virtual ~ItemSpawner() = default;

	virtual void Update()override;
	void Generate()override;

	
private:
	float spawnTimer = 0.0f;
	float spawnTime = 1;
};
