#pragma once
#include "Core/Base.h"

class AbstructButton
{
public:
	AbstructButton();
	~AbstructButton() = default;
	virtual bool update() = 0;
	virtual bool draw()const = 0;
	virtual int getButtonState()const = 0;
	virtual bool setButtonState(int value,bool isSelectEnable)=0;
	virtual bool setMousePoint(POINT* p) = 0;
 	virtual bool checkCollideMouseCursor()=0;
private:

protected:

};
