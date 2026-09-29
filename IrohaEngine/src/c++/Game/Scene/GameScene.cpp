#include "GameScene.h"

GameScene::GameScene(SceneListener* sceneListener, SceneEffectListener* sceneEffectListener, const Parameter& parameter) :
	Scene(sceneListener, sceneEffectListener, parameter)
{
	_sceneEffectListener->setSceneChangeEffect(FadeIn, 1);
	textHandle = 1;
	GameManager::GetInstance().init();
}

GameScene::~GameScene()
{

}

bool GameScene::update()
{

	if (GameManager::GetInstance().IsGameEnd())
	{
		Parameter parameter;
		const bool stackClear = true;
		ILoopController::GetInstance().Cleanup();
		_sceneListener->onScenePush(eScene::Title, parameter, stackClear);
	}
	else
	{
		GameManager::GetInstance().update();
	}

	return true;
}

bool GameScene::draw() const
{
	
	return true;
}
