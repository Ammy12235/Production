#pragma once
#include "ISpawner.h"

class EnemySpawner :public ISpawner
{
public:
	EnemySpawner()=default;
	virtual ~EnemySpawner() = default;

	virtual void Update()override;
	void Generate()override;

private:
	float spawnTimer = 0.0f;
	float spawnTime = 1;
	bool _activateFlag = false;
};
