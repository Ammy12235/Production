#include "TitleMediator.h"

TitleMediator::TitleMediator(SceneListener* sceneListener, SceneEffectListener* sceneEffectListener) :
    _sceneListener(sceneListener), _sceneEffectListener(sceneEffectListener)
{
    CreateParts();
};

TitleMediator::~TitleMediator()
{
    SAFE_DELETE(m_titleMenuList);
    SAFE_DELETE(m_titleTextBox);
}

void TitleMediator::CreateParts()
{
    m_titleMenuList = new TitleMenuList(this,_sceneListener,_sceneEffectListener);
    m_titleTextBox = new TitleTextBox(this);
}

void TitleMediator::DeleteParts()
{

}

void TitleMediator::PartsChanged(Parts* parts)
{
    // ...引数のオブジェクトを判断(検索)
    // ここはif～elseでポインタからオブジェクトを識別します

    if (m_titleMenuList == parts) {
       m_pushedButtonNum = m_titleMenuList->getPushedButtonNum();
       m_selectedButtonNum = m_titleMenuList->getSelctedButtonNum();
       m_titleTextBox->setSelectedNum(m_selectedButtonNum);
    }

}

int TitleMediator::getPushedButtonNum()const
{
	return m_pushedButtonNum;
}

void TitleMediator::update()
{

	//各パーツの更新を行う。何らかの変化があればコールバック関数のPartsChanged関数が呼ばれる
    m_titleMenuList->update();
    m_titleMenuList->draw();

    m_titleTextBox->update();
    m_titleTextBox->draw();
}
