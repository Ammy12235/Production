#include "SceneEffect.h"

/*!
@brief コンストラクタ
@param impl シーン変更のリスナー
@param parameter 前のシーンから渡されたパラメータ
*/
SceneEffect::SceneEffect(SceneEffectListener* listener):
    _sceneEffectListener(listener)
{
}