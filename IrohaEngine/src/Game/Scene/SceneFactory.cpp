#include "SceneFactory.h"
#include "LogoScene.h"
#include "TitleScene.h"
#include "GameScene.h"
#include "ConfigScene.h"
#include "KeyConfigScene.h"


SceneFactory::SceneFactory()
{
	Parameter parameter;
	_sceneEffectListener = &ChangeEffectMover::GetInstance();
	_sceneStack.push(std::make_shared<LogoScene>(this, _sceneEffectListener, parameter)); 
}

SceneFactory::~SceneFactory()
{

}

void SceneFactory::finalize()
{
	while (!_sceneStack.empty()) {//スタックを空にする
		_sceneStack.pop();
	}

}

void SceneFactory::onScenePush(const eScene scene, const Parameter& parameter, const bool stackClear)
{
	if (stackClear) {
		while (!_sceneStack.empty()) {//スタックを空にする

			_sceneStack.pop();
		}
	}
	switch (scene) {
	case Logo:
		_sceneStack.push(std::make_shared<LogoScene>(this, _sceneEffectListener,parameter));
		break;
	case Title:
		_sceneStack.push(std::make_shared<TitleScene>(this, _sceneEffectListener,parameter));
		break;
	case Game:
		_sceneStack.push(std::make_shared<GameScene>(this, _sceneEffectListener, parameter));
		break;
	case Config:
		_sceneStack.push(std::make_shared<ConfigScene>(this, _sceneEffectListener, parameter));
		break;
	case KeyConfig:
		_sceneStack.push(std::make_shared<KeyConfigScene>(this, _sceneEffectListener, parameter));
		break;
	default:
		break;
	}
}
//今のシーンをポップ
void SceneFactory::onScenePop()
{
	_sceneStack.pop();

}
