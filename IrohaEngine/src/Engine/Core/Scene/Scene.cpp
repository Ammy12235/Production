#include "Scene.h"

/*!
@brief コンストラクタ
@param impl シーン変更のリスナー
@param parameter 前のシーンから渡されたパラメータ
*/
Scene::Scene(SceneListener* sceneListener, SceneEffectListener* sceneEffectListener, const Parameter& parameter) :
    _sceneListener(sceneListener),_sceneEffectListener(sceneEffectListener)
{
}