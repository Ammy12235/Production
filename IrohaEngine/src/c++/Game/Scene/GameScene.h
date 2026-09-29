#pragma once

#include <Iroha.h>
#include "Manager/GameManager.h"

//ゲームシーンクラス：ゲーム画面を実行、描画するクラス。
class GameScene : public Scene
{
public:
	const static char* stage;
	const static char* difficulty;

	GameScene(SceneListener* sceneListener, SceneEffectListener* sceneEffectListener, const Parameter& parameter);
	virtual ~GameScene();

	bool update() override;
	bool draw() const override;

private:
	int _level = 0;
	int textHandle = 0;
	std::shared_ptr<GameManager> _gameManager;
};

