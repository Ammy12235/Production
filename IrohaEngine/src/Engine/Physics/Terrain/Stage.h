#pragma once
#include "Core/Base.h"
#include "Core/Entity.h"
#include "eStage.h"

class Entity;

//ステージクラス：ステージのデータと、当たり判定の処理関数を同時に持つクラス。
class Stage
{
private:



	int maptipXCount = 0;
	int maptipYCount = 0;
	int stageId=0;
	int texId=0;
	stgInfo t_stgInfo;
	std::vector<InstanceData2D> instances;
	std::vector<int> mapHitData;
	std::vector<int> mapData;
public:

	virtual void Update();
	virtual void Draw()const;
	bool Load(const LevelData& data);
	bool UnLoad();

	Stage();
	virtual ~Stage() {
		UnLoad();
	};

	stgInfo getStageInfo();
	std::vector<int> getStageHitData();
	bool isHit(int x,int y)const;
	bool isHitSlope(int x, int y)const;
	bool collideStage(Entity& entity, hitInfo& hitInfo, bool* isFloating, bool* isHitSlope, float width, float height);
	friend class StageUtility;
protected:
};

