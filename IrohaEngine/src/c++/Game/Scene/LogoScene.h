#pragma once


#include <Iroha.h>
//ロゴシーンクラス：ロゴ画面を実行、描画するクラス。
class LogoScene : public Scene
{
public:

	LogoScene(SceneListener* sceneListener, SceneEffectListener* sceneEffectListener, const Parameter& parameter);
	virtual ~LogoScene() = default;

	bool update() override;
	bool draw() const override;

private:
	const int fadeInterval = 2;

	bool isFadeInEnd = false;
	bool isFadeOutEnd = false;

	//表示時間を計測するタイマー
	float timer = 0;
};
