#pragma once
#include "Base.h"
#include "Physics/Terrain/Stage.h"
#include "Entity.h"
#include "Factory.h"

class Entity;

//エンティティファクトリークラス。「もの」を作るファクトリークラスの基底クラス。
class EntityFactory:public Factory
{
private:
protected:
	struct mapInfo
	{
		UINT32 id;
		int px, py;
		mapInfo(UINT32 mapId, int x, int y)
		{
			id = mapId;
			px = x; py = y;
		}
	};
	std::list<mapInfo> _mapList;
public:
	EntityFactory() {};
	~EntityFactory() {};
	virtual bool Load(const LevelData& data){ return true; };//外部ファイルからのロード
	virtual bool Create() = 0;//ロードしたものからインスタンスを生成



};
