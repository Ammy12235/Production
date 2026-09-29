#pragma once

#include "Core/Base.h"
#include "Scene.h"
#include "eScene.h"
#include "Parameter.h"
//シーンリスナー：シーンからのシーン遷移依頼を聞くクラス。各シーンはこのクラスを通じてシーン変更要請を出す。
class SceneListener
{
public:
    SceneListener() = default;
    virtual ~SceneListener() = default;
    virtual void onScenePush(const eScene scene, const Parameter& parameter, const bool stackClear) = 0;
    virtual void onScenePop() = 0;
};
