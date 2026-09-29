#include "Fade.h"

void Fade::update()
{
	/*
	//フェードイン処理
	if (isFade)
	{
		fadeTimer += 255 / 60 / fadeInterval;
		alpha -= 0.016f * (1 / fadeInterval);
		buttonFlushTimer++;
		if (buttonFlushTimer > buttonFlushInterval)
		{
			if (Title_Element[selectNum].color == 255)
				Title_Element[selectNum].color = 50;
			else if (Title_Element[selectNum].color == 50)
				Title_Element[selectNum].color = 255;
			buttonFlushTimer = 0.0f;
		}
	}
	if (fadeTimer > 255)
	{
		isFadeEnd = true;
		isFade = false;
		fadeTimer = 0.0f;
	}
	*/
};

void  Fade::draw() const
{

};