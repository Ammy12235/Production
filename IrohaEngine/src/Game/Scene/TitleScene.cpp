#include "TitleScene.h"
#include "GameScene.h"

TitleScene::TitleScene(SceneListener* sceneListener, SceneEffectListener* sceneEffectListener, const Parameter& parameter)
	: Scene(sceneListener, sceneEffectListener, parameter)
{
	_sceneEffectListener->setSceneChangeEffect(FadeIn, 1);
	textHandle[0] = 0;
	textHandle[1] = 1;
	alpha = 0;
	//画像読み込み

	_halTex = TEX_FAC.CreateTexture("tex/player/hal_raf.jpg");
	_meiTex = TEX_FAC.CreateTexture("tex/player/mei_raf.jpg");
	_backGround = TEX_FAC.CreateTexture("tex/background/lowsunny.jpg");
	_rainySound = SND_FAC.CreateSound("snd/se/ambient_rain_0.mp3");
	_crowdySound = SND_FAC.CreateSound("snd/se/ambient_wind_0.mp3");
	_titleBgm = SND_FAC.CreateSound("snd/bgm/title.mp3");
	_cloudTex = TEX_FAC.CreateTexture("tex/background/cloud_001.png");

	EntityManager::GetInstance().AddEntity(bgmEmitter = std::make_shared<SceneBGMEmitter>());
	EntityManager::GetInstance().AddEntity(_ambientSoundEmitter = std::make_shared<AmbientSoundEmitter>(_rainySound));
	RealWeatherManager::GetInstance().Update();
	weatherData = &RealWeatherManager::GetInstance().GetWeatherData();

	bgmEmitter->Play(_titleBgm);
	isFadeOut = false;
	isFadeOutEnd = false;
	isFadeIn = false;
	isFadeInEnd = false;
	CAMERA.SetCameraPos(XMFLOAT2(0, 0));
	CAMERA.SetCameraScl(1);

	for (int i = 0; i < BOX_NUM; i++)
	{
		cloudsX[i] = rand() % (Define::WIN_W + 100);
		cloudsY[i] = rand() % Define::WIN_H/3;

		rcg[i] = rand() % 100;
		rcb[i] = rand() % 100;
	}

	m_titleMediator = new TitleMediator(_sceneListener, _sceneEffectListener);
}

TitleScene::~TitleScene()
{
	TEX_FAC.DeleteTexture(texId[0]);
	SAFE_DELETE(m_titleMediator);
}



bool TitleScene::update()
{
	cloudOffset+=FPS.GetFrameSecondTime();

	weatherSwitchTimer += FPS.GetFrameSecondTime();
	if (weatherSwitchTimer > weatherSwitchTime)
	{
		int index = rand() % 2;
		int soundId = 0;
		if (index == 0)soundId = _crowdySound;
		if (index == 1)soundId = _rainySound;
		_ambientSoundEmitter->SetSoundTransition(soundId, 2);
		weatherSwitchTimer = 0.0f;
	}
	if (isBack)
	{
		_sceneEffectListener->setSceneChangeEffect(FadeIn, 0.5f);
		isBack = false;
	}

	//エフェクトを動かす
	_sceneEffectListener->getFadeInOutContinue(FadeIn, &isFadeIn);
	_sceneEffectListener->getFadeInOutEnd(FadeIn, &isFadeInEnd);
	_sceneEffectListener->getFadeInOutEnd(FadeOut, &isFadeOutEnd);

	m_titleMediator->update();
	m_pushedButtonNum = m_titleMediator->getPushedButtonNum();

	//フェードアウトが済んだらシーン遷移(シーン遷移処理はすべての処理の後にする)
	if (isFadeOutEnd)
	{
		isBack = true;
		isFadeOut = false;
		isFadeOutEnd = false;
		isFadeIn = false;
		isFadeInEnd = false;
		Parameter parameter;
		switch (m_pushedButtonNum)
		{
		case 0:
		{
			_ambientSoundEmitter->Stop();
			_ambientSoundEmitter->Destroy(*_ambientSoundEmitter);
			bgmEmitter->Stop();
			bgmEmitter->Destroy(*bgmEmitter);
			const bool stackClear = true;
			_sceneListener->onScenePush(eScene::Game, parameter, stackClear);

		}
		break;
		/*
		case 1:
		{
			const bool stackClear = false;
			_sceneListener->onScenePush(eScene::Config, parameter, stackClear);
		}
		break;
		*/
		case 1:
		{
			_sceneListener->onScenePop();
			ILoopController::GetInstance().Exit();//ゲームを終了する
		}
		break;
		default:
			break;
		}
	}

	return true;

}

bool TitleScene::draw() const
{
	D3D.SetAlignmentBlendDesc(255);
	DWRITE.DrawFormatText(L"Iroha Engine Demo", 100, 300, 1000, 100, D3D.GetColor(255, 255, 255), 1, 3);
	D3D.SetAddBlendDesc(255);

	D3D.DrawImage(_backGround, 0, 0, Define::WIN_W, Define::WIN_H * 3);

	/*
	D3D.SetSubtractBlendDesc(255);
	D3D.DrawImage(_halTex, 0, 100, 500, 600);
	D3D.DrawImage(_meiTex, 1300, 100, 500, 600);
	D3D.SetDefaultBlendDesc(255);
	*/

	D3D.SetAlignmentBlendDesc(255);
	for (int i = 0; i < BOX_NUM; i++)
	{

		D3D.DrawRotImage(_cloudTex, (int)(cloudsX[i] + (cloudOffset * cloudsY[i])) % (Define::WIN_W + 100) - 50, cloudsY[i] + Define::WIN_H* 2/ 3+sin(cloudsX[i]+cloudOffset)*20
			, 20, 20, (float)cloudsX[i] / 20, (float)cloudsY[i] / 15 + 0.3f);
	}
	D3D.SetDefaultBlendDesc(255);

	WCHAR str[256];
	swprintf(str, 256, L"地域：%hs", weatherData->area.c_str());
	DWRITE.DrawFormatText(str, Define::WIN_W / 2 + 100, Define::WIN_W - 500, 1000, 100, D3D.GetColor(255, 255, 255), 1, 2);
	swprintf(str, 256, L"天気：%hs", weatherData->todayWeather.c_str());
	DWRITE.DrawFormatText(str, Define::WIN_W / 2 + 100, Define::WIN_W - 460, 1000, 100, D3D.GetColor(255, 255, 255), 1, 2);
	swprintf(str, 256, L"降水確率：%hs", weatherData->rainChance.c_str());
	DWRITE.DrawFormatText(str, Define::WIN_W / 2 + 100, Define::WIN_W - 420, 1000, 100, D3D.GetColor(255, 255, 255), 1, 2);


	return true;

}

