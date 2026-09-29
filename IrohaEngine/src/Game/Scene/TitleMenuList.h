#pragma once
#include <Iroha.h>
#include "Parts.h"

class SceneEffectListener;
class SceneListener;

//タイトルメニュークラス：タイトルのメニューバーの更新、描画を行うクラス。
class TitleMenuList :public Parts
{
public:
	TitleMenuList(Mediator* med, SceneListener* sceneListener, SceneEffectListener* sceneEffectListener);
	virtual ~TitleMenuList()=default;
	bool update()override;
	bool draw()const override;
	int getPushedButtonNum()const;
	int getSelctedButtonNum()const;

private:

	bool isSelectedKey = false;//０ならキーボードで選択する

	int m_oldSelectedButtonNum = 0;//前回の選択を保存して比較
	int m_mouseSelectedButtonNum = 0;//最後にマウスで選択した番号を保存
	int m_selectedButtonNum = 0;
	int m_pushedButtonNum = -1;
	static const int selectAllNum = 2;

	bool isFadeOut = false;
	bool isFadeOutEnd = false;
	bool isFadeIn = false;
	bool isFadeInEnd = false;

	bool isTiltedOne = false;

	SceneListener* _sceneListener;
	SceneEffectListener* _sceneEffectListener;

	void AddeElement();

	POINT point;

	typedef struct
	{
		wchar_t name[128];
		int color;
		int type;

	}Title_Element_t;

	Title_Element_t Title_Element[selectAllNum] =
	{
		{L"スタート",255,0},
		{L"ゲーム終了",255,0},

	};
protected:

};
