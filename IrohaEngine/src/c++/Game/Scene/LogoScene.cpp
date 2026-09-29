#include "LogoScene.h"

LogoScene::LogoScene(SceneListener* sceneListener, SceneEffectListener* sceneEffectListener, const Parameter& parameter)
	: Scene(sceneListener, sceneEffectListener, parameter)
{
	
	_sceneEffectListener->setSceneChangeEffect(FadeIn, 2);
	
}

bool LogoScene::update()
{
	
	_sceneEffectListener->getFadeInOutEnd(FadeIn, &isFadeInEnd);
	_sceneEffectListener->getFadeInOutEnd(FadeOut, &isFadeOutEnd);
	timer++;
	if (timer > 240)
	{
		_sceneEffectListener->setSceneChangeEffect(FadeOut, 2);
		timer = 0.0f;
	}
	//フェードイン処理
	if (isFadeOutEnd)
	{
		Parameter parameter;
		const bool stackClear = false;
		_sceneListener->onScenePush(eScene::Title, parameter, stackClear);
	}

	//ボタンが押されたらすぐに次へ
	if (Keyboard::GetInstance().GetKeyDown(DIK_SPACE)
		|| Pad::GetInstance().GetPadDown(ePad::jump)
		|| Mouse::GetInstance().GetMouseDown(0))
	{
		Parameter parameter;
		const bool stackClear = true;
		_sceneListener->onScenePush(eScene::Title, parameter, stackClear);
	}
	return true;

}

bool LogoScene::draw() const
{
	//D3D.DrawCube(100, 100, 1,0,0,0,0,0,0);
	DWRITE.DrawFormatText(L"Iroha Engineデモゲーム", Define::WIN_W / 2-300, Define::WIN_H/2, 1000, 30, D3D.GetColor(200, 130, 0), 1, 2);
	return true;
}
