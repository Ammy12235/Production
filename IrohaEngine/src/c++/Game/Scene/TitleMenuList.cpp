#include "TitleMenuList.h"

TitleMenuList::TitleMenuList(Mediator* med, SceneListener* sceneListener, SceneEffectListener* sceneEffectListener) :
	Parts(med), _sceneListener(sceneListener), _sceneEffectListener(sceneEffectListener)
{
	m_pushedButtonNum = -1;
	//AddeElement();
}

void TitleMenuList::AddeElement()
{
	
}

int TitleMenuList::getPushedButtonNum()const
{
	return m_pushedButtonNum;
};

int TitleMenuList::getSelctedButtonNum()const
{
	return m_selectedButtonNum;
};


bool TitleMenuList::update()
{
	XMFLOAT2 joyStickInput = Pad::GetInstance().GetAnalogStickInput();

	
	if (_sceneEffectListener->getFadeInOutContinue(FadeOut, nullptr) || _sceneEffectListener->getFadeInOutContinue(FadeIn, nullptr))return true;

	//入力による変更
	if (Keyboard::GetInstance().GetKeyDown(DIK_UPARROW)|| Pad::GetInstance().GetPadDown(ePad::up)|| (joyStickInput.y>0&& isTiltedOne==false))
	{
		isTiltedOne = true;
		m_selectedButtonNum = (m_selectedButtonNum + (selectAllNum - 1)) % selectAllNum;
		
		isSelectedKey = true;
		m_Mediator->PartsChanged(this);
	}
	else if (Keyboard::GetInstance().GetKeyDown(DIK_DOWNARROW) || Pad::GetInstance().GetPadDown(ePad::down) || (joyStickInput.y < 0 && isTiltedOne == false))
	{
		isTiltedOne = true;
		m_selectedButtonNum = (m_selectedButtonNum + 1) % selectAllNum;
		
		isSelectedKey = true;
		m_Mediator->PartsChanged(this);
	}

	if (Keyboard::GetInstance().GetKeyDown(DIK_SPACE)||Keyboard::GetInstance().GetKeyDown(DIK_NUMPADENTER) || Pad::GetInstance().GetPadDown(ePad::jump))
	{
		m_pushedButtonNum = m_selectedButtonNum;
		_sceneEffectListener->setSceneChangeEffect(FadeOut, 0.5f);
		
		m_Mediator->PartsChanged(this);
		return true;

	}


	if (Length(joyStickInput) == 0)
	{
		isTiltedOne = false;
	}

	return true;


}


bool TitleMenuList::draw()const
{
	DWRITE.DrawFormatText(L"●", 100, Define::WIN_H / 2 + 100 * m_selectedButtonNum+7, 500, 50, D3D.GetColor(255, 255, 255), 1, 2);

	for (int i = 0; i < selectAllNum; i++)
		DWRITE.DrawFormatText(Title_Element[i].name, 150, Define::WIN_H / 2  + 100 * i, 500, 50, D3D.GetColor(255, 255, 255), 1,2);

	return true;
}
