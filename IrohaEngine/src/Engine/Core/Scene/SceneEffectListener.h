#pragma once

#include "Core/Base.h"
#include "SceneEffect.h"
#include "eSceneChangeEffect.h"

//シーンエフェクトリスナー：各シーンはクラス間での機密性を守る観点からこのクラスを通じてシーン変更要請を出す。
class SceneEffectListener
{
public:
	SceneEffectListener() = default;
	virtual ~SceneEffectListener() = default;
	virtual void setSceneChangeEffect(const eSceneChangeEffect eSceneChangeEffect, float second) = 0;
	virtual bool getFadeInOutEnd(const eSceneChangeEffect eSceneChangeEffect, bool* isEnd)const = 0;
	virtual bool getFadeInOutContinue(const eSceneChangeEffect eSceneChangeEffect, bool* isContinue)const = 0;
};
