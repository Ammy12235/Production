#pragma once
#include <Iroha.h>

enum SpawnerType
{
	SpawnerType_Ceiling,//天井にあるスポナーである
	SpawnerType_Bottom,//床下にあるスポナーである
	SpawnerType_SideLeft,//横にあるスポナーである
	SpawnerType_SideRight,//横にあるスポナーである
	SpawnerType_Middle//真ん中にあるスポナーである
};

enum SpawnerPreset
{
	SpawnPriset_Sunny,
	SpawnPriset_Rainy,
	SpawnPriset_Cloudy
};

class ISpawner :public Entity
{
public:
	ISpawner() = default;
	virtual ~ISpawner() = default;
	virtual void Update() {};
	virtual void Generate()=0;

	void SetType(SpawnerType type)
	{
		_type = type;
	}
	void SetPreset(SpawnerPreset preset)
	{
		_preset = preset;
	}
protected:
	SpawnerType _type;
	SpawnerPreset _preset;
};
