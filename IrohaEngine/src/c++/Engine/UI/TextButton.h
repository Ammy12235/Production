#pragma once
#include "Core/Base.h"
#include "AbstractButton.h"

class TextButton :public AbstructButton
{
public:
	TextButton(unsigned int Id, const WCHAR* text,Rect rect);
	~TextButton(){};
	 bool update()override;
	 bool draw()const override;
	 int getButtonState()const;//0:選択されていない　1:カーソルがあっている　2:押されている
	 bool setButtonState(int value,bool isSelectEnable);
	 bool setMousePoint(POINT* p)override;
	 bool checkCollideMouseCursor()override;
private:
	//ボタンの中身の情報
	Rect m_Rect;
	POINT* point;
	const WCHAR* p_Text=nullptr;
	int color[3]={255,255,255};
	float alpha = 1;
	unsigned int Id = 0;

	//選択に関するフラグ
	bool isSelected=false;
	bool isPushed=false;
	bool isSelectEnable = true;//マウスカーソルで選択可能か
};
