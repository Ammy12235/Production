#pragma once
#include <Iroha.h>
#include "Mediator.h"

class Mediator;

//Partsクラス：Mediatorクラスの部品クラス。
class Parts
{
protected:
	Mediator* m_Mediator;
public:
	virtual bool update() = 0;
	virtual bool draw()const = 0;
	Parts(Mediator* mediator = nullptr) : m_Mediator(mediator)
	{

	};  // コンストラクタでMediatorを登録
};
