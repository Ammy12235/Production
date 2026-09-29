#include "ConfigScene.h"
#include "TitleScene.h"


ConfigScene::ConfigScene(SceneListener* sceneListener, SceneEffectListener* sceneEffectListener, const Parameter& parameter) :
	Scene(sceneListener, sceneEffectListener, parameter)
{
	textHandle[0] = 0; textHandle[1] = 1;
	_sceneEffectListener->setSceneChangeEffect(FadeIn, 0.5f);

	for (int i = 0; i < BOX_NUM; i++)
	{
		rx[i] = rand() % (Define::WIN_W + 100);
		ry[i] = rand() % Define::WIN_H;

		rcr[i] = rand() % 100;
		rcb[i] = rand() % 100;
	}

	alpha = 1;
	isFadeOut = false;
	isFadeOutEnd = false;
	isFadeIn = false;
	isFadeInEnd = false;
}

bool ConfigScene::update()
{
	//エフェクトを動かす
	_sceneEffectListener->getFadeInOutContinue(FadeOut, &isFadeOut);
	_sceneEffectListener->getFadeInOutEnd(FadeIn, &isFadeInEnd);
	_sceneEffectListener->getFadeInOutEnd(FadeOut, &isFadeOutEnd);
	
	c++;

	//フェードアウトが済んだらシーン遷移
	if (isFadeOutEnd)
	{
		isFadeOut = false;
		isFadeOutEnd = false;
		isFadeIn = false;
		isFadeInEnd = false;
		_sceneListener->onScenePop();
		
		return true;
	}

	if (_sceneEffectListener->getFadeInOutContinue(FadeOut, nullptr) || _sceneEffectListener->getFadeInOutContinue(FadeIn, nullptr))return true;

	//キーボードの操作
	if (Keyboard::GetInstance().GetKeyDown(DIK_UPARROW))
	{
		selectNum = (selectNum + (selectDetailAllNum - 1)) % selectDetailAllNum;
	}
	else if (Keyboard::GetInstance().GetKeyDown(DIK_DOWNARROW))
	{
		selectNum = (selectNum + 1) % selectDetailAllNum;
	}

	if (Keyboard::GetInstance().GetKeyDown(DIK_SPACE)&&selectNum==5)
	{
		_sceneEffectListener->setSceneChangeEffect(FadeOut, 1);
		//SOUNDS.PlaySoundW(0, 0);

	}
	return true;
}

bool ConfigScene::draw() const
{
	/*
	DWRITE.DrawFormatText(L"Config", 100, 100, 200, 30, D3D.GetColor(255, 255, 255), alpha, textHandle[0]);
	DWRITE.DrawFormatText(L"O", 280, 200 + 50 * selectNum, 200, 50, D3D.GetColor(255, 255, 255), alpha, textHandle[1]);

	for (int i = 0; i < selectAllNum; i++)
		DWRITE.DrawFormatText(Config_Element[i].name, 150, 200 + 100 * i, 200, 30, D3D.GetColor(255, 255, 255), alpha, textHandle[1]);

	for (int i = 0; i < selectDetailAllNum; i++)
		DWRITE.DrawFormatText(Config_Detail_Element[i].name, 300, 200 + 50 * i, 200, 30, D3D.GetColor(255, 255, 255), alpha, textHandle[1]);

	for (int i = 0; i < BOX_NUM; i++)
	{
		D3D.DrawRotBox((rx[i] + (c * ry[i] / 200)) % (Define::WIN_W + 100) - 50, ry[i]
			, 20, 20, (float)rx[i] / 20, (float)ry[i] / 300 + 0.3f, D3D.GetColor(100 + rcr[i], 255, 100 + rcb[i]));

	}
	*/

	return true;
}
