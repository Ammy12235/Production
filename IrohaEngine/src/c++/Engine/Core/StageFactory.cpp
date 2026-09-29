#include "StageFactory.h"
/*
bool StageFactory::Load(int stageId ,std::list<std::shared_ptr<Stage>>& _stage)
{
	if (Stage_None + 1 < stageId || stageId > Stage_Max)return false;//存在しないためロードをスキップ
	for (auto it = _stage.begin(); it != _stage.end();) {
		if ((*it)->getStageInfo().stageId == stageId) {
			return true;//すでに生成済みのためロードをスキップ
		}
		else {
			it++;
		}
	}

	//ステージのインスタンスを生成
	_stage.push_back(std::make_shared<Stage>());
	_stage.back()->load(stageId);

	return true;
}
*/

/*
bool StageFactory::unload(int stageId)
{
	for (auto it = _stageList.begin(); it != _stageList.end();) {
		if ((*it)->getStageInfo()->stageId == stageId) {
			_stageList.erase(it);
			return true;
		}
		else {
			it++;
		}
	}

	return false;
}
*/
