#pragma once
#include "Factory.h"
#include "Physics/Terrain/Stage.h"
class Stage;

//ステージファクトリークラス：ステージを生成するクラス。正確にはこのクラスがステージのインスタンスを作り、各ステージがデータをロードをする。
class StageFactory:public Factory
{
private:

public:
	//bool unload(int stageId);
	//bool load(int stageId, std::list<std::shared_ptr<Stage>>& _stage);
};
