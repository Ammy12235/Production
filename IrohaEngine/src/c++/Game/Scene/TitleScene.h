#pragma once

#include <Iroha.h>
#include "State.h"
#include "TitleMediator.h"

#include "General/SceneBGMEmitter.h"
#include "General/AmbientSoundEmitter.h"

#include "Module/Climate/RealWeather.h"

#define BOX_NUM 500

//タイトルシーンクラス：タイトル画面を実行、描画する具象クラス。タイトル画面の表示をする。
class TitleScene : public Scene
{
public:
	TitleScene(SceneListener* sceneListener,SceneEffectListener* sceneEffectListener, const Parameter& parameter);
	virtual ~TitleScene();
	bool update() override;
	bool draw() const override;

private:

	TitleMediator* m_titleMediator = nullptr;

	int textHandle[2] = { 0 };
	int texId[5] = { 0 };
	float alpha = 0;

	bool isFadeOut = false;
	bool isFadeOutEnd = false;
	bool isFadeIn = false;
	bool isFadeInEnd = false;
	bool isBack = false;

	int m_pushedButtonNum = -1;

	int _halTex = 0;
	int _meiTex = 0;
	int _cloudTex = 0;
	int _backGround = 0;
	int _rainySound = 0;
	int _crowdySound = 0;
	int _sunnySound = 0;
	int _icicleSound = 0;
	int _titleBgm = 0;

	float cloudOffset = 0;
	int cloudsX[BOX_NUM];
	int cloudsY[BOX_NUM];
	int rcg[BOX_NUM];
	int rcb[BOX_NUM];

	float weatherSwitchTimer = 0;
	float weatherSwitchTime = 10;
	float weatherSwitchSpeedTime = 10;
	std::shared_ptr<AmbientSoundEmitter> _ambientSoundEmitter = nullptr;
	std::shared_ptr<SceneBGMEmitter> bgmEmitter=nullptr;

	RealWeatherData* weatherData=nullptr;
};
