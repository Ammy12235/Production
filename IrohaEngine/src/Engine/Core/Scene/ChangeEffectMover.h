#pragma once
#include "Core/Singleton.h"
#include "SceneEffectListener.h"
#include "eSceneChangeEffect.h"

class ChangeEffectMover :public Singleton<ChangeEffectMover>,public SceneEffectListener
{
public:
	ChangeEffectMover();
	~ChangeEffectMover();
	void setSceneChangeEffect(const eSceneChangeEffect eSceneChangeEffect, float second)override;
	bool getFadeInOutEnd(const eSceneChangeEffect eSceneChangeEffect,bool* isEnd)const override;
	bool getFadeInOutContinue(const eSceneChangeEffect eSceneChangeEffect, bool* isContinue)const override;
	void update();
	void draw()const;
	void clear();
private:
	//遷移にかかる時間とタイマー
	float fadeTimer = 0.0f;
	float fadeInterval = -1;
	float alpha = 0;

	int texId;

	//現在の状態を示すフラグ
	bool isFadeOut = false;
	bool isFadeOutEnd = false;
	bool isFadeIn = false;
	bool isFadeInEnd = false;

};
