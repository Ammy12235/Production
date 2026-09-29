#pragma once
#include <Iroha.h>

#include "Parts.h"
#include "Mediator.h"
#include "TitleMenuList.h"
#include "TitleTextBox.h"

class SceneListener;
class SceneEffectListener;

//タイトルMediator：タイトル画面にあるものを管理するクラス。
class TitleMediator :public Mediator
{

public:
	TitleMediator(SceneListener*  sceneListener,SceneEffectListener* sceneEffectListener);
	~TitleMediator();
	void update()override;    // 画面更新

	// 変化したパーツに対する振る舞いを定義
	void PartsChanged(Parts*)override;
	int getPushedButtonNum()const;

private:

	int m_selectedButtonNum = 0;
	int m_pushedButtonNum = -1;

	SceneListener* _sceneListener;
	SceneEffectListener* _sceneEffectListener;

	TitleMenuList* m_titleMenuList = nullptr;
	TitleTextBox* m_titleTextBox = nullptr;

	bool isPushedButton = false;
	void CreateParts()override;    // パーツを生成

	void DeleteParts()override;
};
