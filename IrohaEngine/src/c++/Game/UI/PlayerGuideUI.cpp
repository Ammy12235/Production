#include "PlayerGuideUI.h"

void PlayerGuideUI::DrawPlayerGuide(UINT currentHP, UINT maxHP, UINT mainCurrentEP, UINT mainMaxEP, UINT subCurrentEP, UINT subMaxEP,
	bool isMainFlowActivated, bool isSubFlowActivated, bool isCoopEnabled, bool isMainIndivEnabled, bool isSubIndivEnabled)const
{
	//UIの描画
	D3D.SetLayer(Layer_UI_0);
	//ガイドボタンのサイズや位置を決める
	XMFLOAT2 hpBarPos = { 40,(float)Define::WIN_H - 100 };
	XMFLOAT2 hpBarSize = { 500,50 };

	XMFLOAT2 mainEpBarPos = { 40,(float)Define::WIN_H - 200 };
	XMFLOAT2 mainEpBarSize = { 300,50 };
	XMFLOAT2 subEpBarPos = { 40,(float)Define::WIN_H - 150 };
	XMFLOAT2 subEpBarSize = { 300,50 };

	// Xboxの配置に合わせ、左X・上Y・右Bに並べる。
	XMFLOAT2 buttonGuideCoopPos = { (float)Define::WIN_W - 300,(float)Define::WIN_H - 250 };
	XMFLOAT2 buttonGuideIndivMainPos = { (float)Define::WIN_W - 200,(float)Define::WIN_H - 150 };
	XMFLOAT2 buttonGuideIndivSubPos = { (float)Define::WIN_W - 400,(float)Define::WIN_H - 150 };
	XMFLOAT2 buttonGuideSize = { 100,100 };

	const float controlGuideSize = 32.0f;
	const float epGuideGap = 12.0f;
	// guideUI_002.pngは32px四方で、左からX・B・Y・LT・RT。
	const auto drawControlGuide = [&](float x, float y, int column)
	{
		D3D.DrawDivImage(_controlGuideTexture, x, y, controlGuideSize, controlGuideSize,
			32 * column, 0, 32, 32);
	};
	const auto drawAttackControlGuide = [&](const XMFLOAT2& position, int column)
	{
		// 技の絵を残しつつ、アイコンの下部内側に収める。
		drawControlGuide(position.x + (buttonGuideSize.x - controlGuideSize) * 0.5f,
			position.y + buttonGuideSize.y - controlGuideSize - 8.0f, column);
	};

	float innerSize = 15;
	float visualHp = (float)currentHP / (float)maxHP; if (visualHp < 0)visualHp = 0;

	D3D.DrawDivImage(_guideAndStatusTexture, hpBarPos.x, hpBarPos.y, hpBarSize.x, hpBarSize.y, 0, 0, 16 * 14, 16 * 2);//HPバーの背景
	D3D.DrawDivImage(_guideAndStatusTexture, hpBarPos.x + innerSize, hpBarPos.y + innerSize,
		(hpBarSize.x - innerSize * 2) * visualHp,
		hpBarSize.y - innerSize * 2, 16 * 8, 16 * 2, 16, 16);//HPバー

	innerSize = 15;
	float visualEp = (float)mainCurrentEP / (float)mainMaxEP; if (visualEp < 0)visualEp = 0;
	D3D.DrawDivImage(_guideAndStatusTexture, mainEpBarPos.x, mainEpBarPos.y, mainEpBarSize.x, mainEpBarSize.y, 0, 16 * 2, 16 * 8, 16 * 2);//メインのEPバーの背景
	if (isMainFlowActivated)
	{
		D3D.SetAddBlendDesc(200);
		D3D.DrawBox(mainEpBarPos.x, mainEpBarPos.y, mainEpBarSize.x, mainEpBarSize.y, D3D.GetColor(100, 100, 100));//EPバーのフロー状態の背景
		D3D.SetDefaultBlendDesc(255);
	}
	D3D.DrawDivImage(_guideAndStatusTexture, mainEpBarPos.x + innerSize, mainEpBarPos.y + innerSize,
		(mainEpBarSize.x - innerSize * 2) * visualEp,
		mainEpBarSize.y - innerSize * 2, 16 * 8, 16 * 3, 16, 16);//EPバーMain

	visualEp = (float)subCurrentEP / (float)subMaxEP; if (visualEp < 0)visualEp = 0;
	D3D.DrawDivImage(_guideAndStatusTexture, subEpBarPos.x, subEpBarPos.y, subEpBarSize.x, subEpBarSize.y, 0, 16 * 4, 16 * 8, 16 * 2);//EPバーの背景
	if (isSubFlowActivated)
	{
		D3D.SetAddBlendDesc(200);
		D3D.DrawBox(subEpBarPos.x, subEpBarPos.y, subEpBarSize.x, subEpBarSize.y, D3D.GetColor(100, 100, 100));//サブのEPバーのフロー状態の背景
		D3D.SetDefaultBlendDesc(255);
	}
	D3D.DrawDivImage(_guideAndStatusTexture, subEpBarPos.x + innerSize, subEpBarPos.y + innerSize,
		(subEpBarSize.x - innerSize * 2) * visualEp,
		subEpBarSize.y - innerSize * 2, 16 * 9, 16 * 3, 16, 16);//EPバーSub

	if (isMainFlowActivated)
	{
		drawControlGuide(mainEpBarPos.x + mainEpBarSize.x + epGuideGap,
			mainEpBarPos.y + (mainEpBarSize.y - controlGuideSize) * 0.5f, 3);//LT
	}
	if (isSubFlowActivated)
	{
		drawControlGuide(subEpBarPos.x + subEpBarSize.x + epGuideGap,
			subEpBarPos.y + (subEpBarSize.y - controlGuideSize) * 0.5f, 4);//RT
	}

	if (isCoopEnabled)
	{
		D3D.SetAlignmentBlendDesc(255);//押せるならば強調表示を行う
	}
	else
	{
		D3D.SetAlignmentBlendDesc(100);
	}
	D3D.DrawDivImage(_guideAndStatusTexture, buttonGuideCoopPos.x, buttonGuideCoopPos.y, buttonGuideSize.x, buttonGuideSize.y, 0, 16 * 6, 16 * 4, 16 * 4);//協力攻撃ガイド
	drawAttackControlGuide(buttonGuideCoopPos, 2);//Y: 協力攻撃
	D3D.SetDefaultBlendDesc(255);


	if (isMainIndivEnabled)
	{
		D3D.SetAlignmentBlendDesc(255);//押せるならば強調表示を行う
	}
	else
	{
		D3D.SetAlignmentBlendDesc(100);
	}
	D3D.DrawDivImage(_guideAndStatusTexture, buttonGuideIndivMainPos.x, buttonGuideIndivMainPos.y, buttonGuideSize.x, buttonGuideSize.y, 16 * 4, 16 * 6, 16 * 4, 16 * 4);//メイン個人攻撃ガイド
	drawAttackControlGuide(buttonGuideIndivMainPos, 1);//B: メイン個人攻撃
	D3D.SetDefaultBlendDesc(255);


	if (isSubIndivEnabled)
	{
		D3D.SetAlignmentBlendDesc(255);//押せるならば強調表示を行う
	}
	else
	{
		D3D.SetAlignmentBlendDesc(100);
	}
	D3D.DrawDivImage(_guideAndStatusTexture, buttonGuideIndivSubPos.x, buttonGuideIndivSubPos.y, buttonGuideSize.x, buttonGuideSize.y, 16 * 8, 16 * 6, 16 * 4, 16 * 4);//サブ個人攻撃ガイド
	drawAttackControlGuide(buttonGuideIndivSubPos, 0);//X: サブ個人攻撃
	D3D.SetDefaultBlendDesc(255);

	D3D.SetLayer(Layer_0);
}
