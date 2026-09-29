#pragma once

#include "Core/Base.h"
#include "SceneListener.h"
#include "SceneEffectListener.h"
#include "Parameter.h"


class SceneListener;
class SceneEffectListener;

//シーンクラス：各シーンのインターフェースクラス。
class Scene:public CELEMENT
{
protected:
    SceneListener* _sceneListener;
    SceneEffectListener* _sceneEffectListener;
public:
    Scene(SceneListener* sceneListener,SceneEffectListener* sceneEffectListener, const Parameter& parameter);
    virtual ~Scene() = default;
    virtual bool update() = 0;
    virtual bool draw()const = 0;
};
