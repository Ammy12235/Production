#pragma once
#include <Iroha.h>

class PlayerGuideUI
{

private:

	float guideAppearTime = 0.2;
	float guideAppearKeep = 0.6;
	float guideAppearDisapeear = 0.2;
	//技が使用可能になる瞬間に出るエフェクトのタイマー
	float guideCoopEnabledTimer = 0.0f;
	float guideMainIndivEnabledTimer = 0.0f;
	float guideSubIndivEnabledTimer = 0.0f;

	//技が出るかを保存する変数
	bool preIsCoopEnabled = false;
	bool preIsMainIndivEnabled = false;
	bool preIsSubIndivEnabled = false;
public:
	int _guideAndStatusTexture = 0;
	int _controlGuideTexture = 0;

	PlayerGuideUI()
	{
		_guideAndStatusTexture = TEX_FAC.CreateTexture("tex/ui/guideUI_001.png");
		_controlGuideTexture = TEX_FAC.CreateTexture("tex/ui/guideUI_002.png");
	}

	virtual ~PlayerGuideUI()
	{
		
	}

	void DrawPlayerGuide(UINT currentHP, UINT maxHP, UINT mainCurrentEP, UINT mainMaxEP, UINT subCurrentEP, UINT subMaxEP,
		bool isMainFlowActivated, bool isSubFlowActivated, bool isCoopEnabled, bool isMainIndivEnabled, bool isSubIndivEnabled)const;
	

};
