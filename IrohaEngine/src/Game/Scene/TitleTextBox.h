#pragma once
#include <Iroha.h>
#include "Parts.h"

//タイトルテキストボックスクラス：タイトルのテキストを更新、描画するクラス。
class TitleTextBox :public Parts
{
public:
	TitleTextBox(Mediator* med);
	virtual ~TitleTextBox() = default;
	bool update()override;
	bool draw()const override;
	bool setSelectedNum(int value);
private:
	int selectedNum=0;
    int oldSelectedNum = 0;
	float textAlpha = 1;

    bool isDissolve=false;
    const float dissolveInterval = 0.7f;//Second
    float dissolveTime = 0.0f;

    typedef struct
    {
        wchar_t name[128];
        int color;
    }Text_Element_t;

    Text_Element_t Text_Element[2] =
    {
        {L"ゲームを開始します",255},
        {L"ゲームを終了します",255},

    };
};
