#pragma once

#include "SceneEffectListener.h"
#include "Core/Base.h"


class SceneEffectListener;

//シーンエフェクトクラス：シーンが遷移するときのエフェクトのインターフェースクラス。
class SceneEffect :public CELEMENT
{
protected:
    SceneEffectListener* _sceneEffectListener;
public:
    SceneEffect(SceneEffectListener* listener);
    virtual ~SceneEffect() = default;
    virtual void update() = 0;
    virtual void draw()const = 0;
};
