#pragma once
#include "Core/Singleton.h"
#include "Core/Scene/Scene.h"
#include "Core/Scene/SceneListener.h"
#include "Core/Scene/SceneEffectListener.h"
#include "Core/Scene/ChangeEffectMover.h"

//シーンファクトリークラス：シーンを生成、削除するクラス。シーンをスタック形式で保存する。
class SceneFactory :public Singleton<SceneFactory>, public SceneListener
{
public:
    SceneFactory();
    ~SceneFactory();

    void finalize();
    void onScenePush(const eScene scene, const Parameter & parameter, const bool stackClear) override;//シーンの生成
    void onScenePop() override;//シーンの削除

	std::stack<std::shared_ptr<Scene>> _sceneStack; //シーンのスタック
    SceneEffectListener* _sceneEffectListener = nullptr;
private:
};
