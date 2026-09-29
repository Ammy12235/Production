#include "TitleTextBox.h"

TitleTextBox::TitleTextBox(Mediator* med) : Parts(med)
{

}


bool TitleTextBox::setSelectedNum(int value)
{
	selectedNum = value;
	return true;
}

bool TitleTextBox::update()
{
	if (selectedNum != oldSelectedNum)
	{
		oldSelectedNum = selectedNum;
		isDissolve = true;
		textAlpha = 0; 
		dissolveTime = 0.0f;
	
	}
	if (isDissolve)
	{
		dissolveTime++;
		textAlpha += (float)(1 / (Define::GameFPS*dissolveInterval));

		if (dissolveTime > Define::GameFPS *dissolveInterval)
		{
			isDissolve = false;
			dissolveTime = 0.0f;
			
		}
	}
	return true;
}

bool TitleTextBox::draw()const
{
	
	DWRITE.DrawFormatText(Text_Element[selectedNum].name, 130, Define::WIN_H-200-(10 * (pow((1 - textAlpha), 2))), 500, 30, D3D.GetColor(0,0,0), textAlpha, 2);

	return true;
}
