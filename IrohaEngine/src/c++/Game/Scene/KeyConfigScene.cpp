#include "KeyConfigScene.h"
#include "TitleScene.h"

KeyConfigScene::KeyConfigScene(SceneListener* sceneListener, SceneEffectListener* sceneEffectListener, const Parameter& parameter) :
	Scene(sceneListener, sceneEffectListener, parameter)
{
	textHandle[0] = 0; textHandle[1] = 1; alpha = 1;
	_sceneEffectListener->setSceneChangeEffect(FadeIn, 2);
	for (int i = 0; i < BOX_NUM; i++)
	{
		rx[i] = rand() % (Define::WIN_W + 100);
		ry[i] = rand() % Define::WIN_H;

		rcg[i] = rand() % 100;
		rcb[i] = rand() % 100;
	}
}

bool KeyConfigScene::update()
{
	if (Pad::GetInstance().GetPadDown(ePad::up))
	{
		selectNum = (selectNum + 8) % 9;
	}
	else if (Pad::GetInstance().GetPadDown(ePad::down))
	{
		selectNum = (selectNum + 1) % 9;
	}
	else if (Pad::GetInstance().GetPadDown(ePad::jump))
	{
		_sceneListener->onScenePop();
	}
	c++;
	return true;
}

bool KeyConfigScene::draw() const
{
	DWRITE.DrawFormatText(L"KeyConfig", 100, 100, 200, 30, D3D.GetColor(255, 255, 255), alpha, textHandle[0]);
	DWRITE.DrawFormatText(L"O", 280, 200 + 50 * selectNum, 200, 50, D3D.GetColor(255, 255, 255), alpha, textHandle[1]);

	for (int i = 0; i < 9; i++)
		DWRITE.DrawFormatText(KeyConfig_Element[i].name, 300, 200 + 50 * i, 200, 30, D3D.GetColor(255, 255, 255), alpha, textHandle[1]);

	for (int i = 0; i < BOX_NUM; i++)
	{
		D3D.DrawRotBox((rx[i] + (c * ry[i] / 200)) % (Define::WIN_W + 100) - 50, ry[i]
			, 20, 20, (float)rx[i] / 20, (float)ry[i] / 300 + 0.3f, D3D.GetColor(255, 100 + rcg[i], 100 + rcb[i]));

	}
	return true;
}
