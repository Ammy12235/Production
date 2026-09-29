#pragma once

#include "Core/Base.h"

class mapTip
{
private:

protected:

public:
	mapTip();
	virtual ~mapTip();
	
	void PlayMapTipSound();
	XMFLOAT2 getMapTipUV()const;
};
