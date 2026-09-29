#include "GameManager.h"

enum eStage
{
	Stage_None = -1,
	Stage_1_1,
	Stage_ScoreMode,
	Stage_1_3,
	Stage_1_4,
	Stage_1_5,
	Stage_Max,
};

LevelData levels[2] =
{
	{
		"stg/stgmap_000.csv",
		"stg/stghitmap_000.csv",
		"stg/enemy_1_1.csv",
		"stg/item_1_1.csv",
		"tex/stage/map_000.png",
		20,
		20,
	},

	{
		"stg/stage_scoremode_visual.csv",
		"stg/stage_scoremode_collision.csv",
		"stg/enemy_1_1.csv",
		"stg/item_1_1.csv",
		"tex/stage/map_000.png",
		20,
		20,
	},
};

GameManager::GameManager()
{
	_scoreBoard   = TEX_FAC.CreateTexture("tex/ui/scoreUI_001.png");
	_weatherBoard = TEX_FAC.CreateTexture("tex/ui/weatherUI_001.png");
	_weatherIcon  = TEX_FAC.CreateTexture("tex/ui/weather_icon.png");
	_sunnyTex     = TEX_FAC.CreateTexture("tex/background/verysunny.jpg");
	_leafTex      = TEX_FAC.CreateTexture("tex/leaf.png");
	_cloudTex     = TEX_FAC.CreateTexture("tex/background/cloud_001.png");

	_sunnyBgm[0] = SND_FAC.CreateSound("snd/bgm/sunny_0.mp3");
	_sunnyBgm[1] = SND_FAC.CreateSound("snd/bgm/sunny_1.mp3");
	_sunnyBgm[2] = SND_FAC.CreateSound("snd/bgm/sunny_2.mp3");
	_rainyBgm[0] = SND_FAC.CreateSound("snd/bgm/rainy_0.mp3");
	_rainyBgm[1] = SND_FAC.CreateSound("snd/bgm/rainy_1.mp3");
	_rainyBgm[2] = SND_FAC.CreateSound("snd/bgm/rainy_2.mp3");

	_openingFallSE   = SND_FAC.CreateSound("snd/se/ambient_wind_0.mp3");
	_openingLandSE   = SND_FAC.CreateSound("snd/se/down_ground.wav");
	_showResultSE    = SND_FAC.CreateSound("snd/se/wind_inst_0.wav");
	_gameOverSE      = SND_FAC.CreateSound("snd/se/gameover.wav");
	_weatherChangeSE = SND_FAC.CreateSound("snd/se/weather_change.wav");
	_waveClearSE     = SND_FAC.CreateSound("snd/se/wave_clear.wav");
	_climateChangeTex       = TEX_FAC.CreateTexture("tex/ui/climateUI_001.png");
	_climateChange_arrowTex = TEX_FAC.CreateTexture("tex/ui/climate_arrowUI.png");
	_tillendTex             = TEX_FAC.CreateTexture("tex/ui/tillEndUI.png");
	_clearTex               = TEX_FAC.CreateTexture("tex/ui/clearUI_001.png");



}

GameManager::~GameManager()
{

}

void GameManager::init()
{
	//ステージのロード

	FactoryManager::GetInstance().Load(levels[Stage_ScoreMode]);

	//敵のスポナーを作成

	EntityManager::GetInstance().AddEntity(_leftEnemySpawner = std::make_shared<EnemySpawner>());
	EntityManager::GetInstance().AddEntity(_rightEnemySpawner = std::make_shared<EnemySpawner>());
	EntityManager::GetInstance().AddEntity(_ceilingEnemySpawner = std::make_shared<EnemySpawner>());
	EntityManager::GetInstance().AddEntity(_bottomEnemySpawner = std::make_shared<EnemySpawner>());
	EntityManager::GetInstance().AddEntity(_middleEnemySpawner_0 = std::make_shared<EnemySpawner>());
	EntityManager::GetInstance().AddEntity(_middleEnemySpawner_1 = std::make_shared<EnemySpawner>());
	EntityManager::GetInstance().AddEntity(_middleEnemySpawner_2 = std::make_shared<EnemySpawner>());

	_spawner.emplace_back(_leftEnemySpawner);
	_spawner.emplace_back(_rightEnemySpawner);
	_spawner.emplace_back(_ceilingEnemySpawner);
	_spawner.emplace_back(_bottomEnemySpawner);
	_spawner.emplace_back(_middleEnemySpawner_0);
	_spawner.emplace_back(_middleEnemySpawner_1);
	_spawner.emplace_back(_middleEnemySpawner_2);

	_middleSpawners.emplace_back(_middleEnemySpawner_0);
	_middleSpawners.emplace_back(_middleEnemySpawner_1);
	_middleSpawners.emplace_back(_middleEnemySpawner_2);

	_sideSpawners.emplace_back(_leftEnemySpawner);
	_sideSpawners.emplace_back(_rightEnemySpawner);

	_upDownSpawners.emplace_back(_ceilingEnemySpawner);
	_upDownSpawners.emplace_back(_bottomEnemySpawner);

	_leftEnemySpawner->SetType(SpawnerType_SideLeft);
	_rightEnemySpawner->SetType(SpawnerType_SideRight);
	_ceilingEnemySpawner->SetType(SpawnerType_Ceiling);
	_bottomEnemySpawner->SetType(SpawnerType_Bottom);
	_middleEnemySpawner_0->SetType(SpawnerType_Middle);
	_middleEnemySpawner_1->SetType(SpawnerType_Middle);
	_middleEnemySpawner_2->SetType(SpawnerType_Middle);

	_leftEnemySpawner->SetPosition(XMFLOAT2(-500, 1000));
	_rightEnemySpawner->SetPosition(XMFLOAT2(32 * 64 + 500, 1000));
	_ceilingEnemySpawner->SetPosition(XMFLOAT2(1064, -900));

	_middleEnemySpawner_0->SetPosition(XMFLOAT2(1064, 800));
	_middleEnemySpawner_1->SetPosition(XMFLOAT2(1064 - 900, 1000));
	_middleEnemySpawner_2->SetPosition(XMFLOAT2(1064 + 800, 1000));

	//アイテムのスポナーを作成

	EntityManager::GetInstance().AddEntity(_bottomItemSpawner = std::make_shared<ItemSpawner>());
	EntityManager::GetInstance().AddEntity(_ceilingItemSpawner = std::make_shared<ItemSpawner>());

	_spawner.emplace_back(_bottomItemSpawner);
	_spawner.emplace_back(_ceilingItemSpawner);


	_bottomItemSpawner->SetType(SpawnerType_Bottom);
	_ceilingItemSpawner->SetType(SpawnerType_Ceiling);

	//天気モジュールの生成

	_weathers.push_back(std::make_shared<Sunny>());
	_weathers.push_back(std::make_shared<Rainy>());
	_weathers.push_back(std::make_shared<Cloudy>());

	EntityManager::GetInstance().AddEntity(_weathers[0]);
	EntityManager::GetInstance().AddEntity(_weathers[1]);
	EntityManager::GetInstance().AddEntity(_weathers[2]);

	//つねに常駐するモジュールを生成

	EntityManager::GetInstance().AddEntity(_atmosphere = std::make_shared<Atmosphere>(XMFLOAT2(-2000, -2000), XMFLOAT2(6000, 6000)));

	EntityManager::GetInstance().AddEntity(_openingSEEmitter = std::make_shared<SceneSEEmitter>());
	EntityManager::GetInstance().AddEntity(_bgmEmitter = std::make_shared<SceneBGMEmitter>());
	EntityManager::GetInstance().AddEntity(_seEmitter = std::make_shared<SceneSEEmitter>());

	EntityManager::GetInstance().AddEntity(_deathZone = std::make_shared<DeathZone>(XMFLOAT2(1064, 2632)));
	EntityManager::GetInstance().AddEntity(_enemyEraser = std::make_shared<EnemyEraser>(XMFLOAT2(1064, 800)));

	_deathZone->SetDeathZoneSize(XMFLOAT2(10000, 100));
	_enemyEraser->SetEnemyEraserSize(XMFLOAT2(10000, 10000));
	_enemyEraser->SetActivate(false);
	_openingSEEmitter->Play(_openingFallSE);//最初の風切り音

	swprintf(gameOverStr, 256, L"GAME OVER!");

	_atmosphere->SetActiveSimulation(true);
	backGroundPos_Layer5 = { -1000.0f, -1000.0f };
	backGroundPos_Layer4 = { 200.0f, 200.0f };
	cameraPallaxPos = { 0,0 };
	preCameraPallaxPos = { 0,0 };
	isReturnTitle = false;
	isFirstEntry = true;

	score.defeatedNum = 0;
	score.playerTotalDamage = 0;
	score.enemyTotalDamage = 0;
	score.waveNum = 0;
	score.rank = 0;

	currentQuotaValue = 0;
	destQuota = 0;
	waveTimer = 0.0f;

	_weatherState = WeatherState_Sunny;
	_scoreModeState = ScoreModeState_Opening;
	_openingState = OpeningState_GameGuide_Delay;

	_weatherChangeState = WeatherChangeState_WeatherGuide_Appear;
	_waveState = WaveStete_WaveCount_Appear;
	_clearState = ClearState_ClearGuide_Appear;
	_gameoverState = GameOverState_Guide_Delay;
	_resultState = ResultState_Guide_Appear;
	_gmTimer = 0.0f;

	openingGuideDelayTime = 4.0f;
	openingGuideAppearTime = 0.4f;
	openingGuideKeepTime = 3.0f;
	openingGuideDisappearTime = 0.5f;

	openingWaveCountAppearTime = 0.4f;
	openingWaveCountKeepTime = 4.0f;
	openingWaveCountDisappearTime = 0.4f;

	weatherChangeGuideAppearTime = 0.4f;
	weatherChangeGuideKeepTime = 4.0f;
	weatherChangeGuideDisappearTime = 0.4f;


	clearGuideAppearTime = 0.4f;
	clearGuideKeepTime = 3;
	clearGuideDisappearTime = 0.4f;

	clearScoreCountAppearTime = 0.4f;
	clearScoreCountKeepTime = 3;
	clearScoreCountDisappearTime = 0.4f;


	gameOverGuideAppearTime = 0.4f;
	gameOverGuideKeepTime = 3.0f;
	gameOverGuideDisappearTime = 0.4f;


	resultScoreAppearTime = 2.0f;
	resultScoreKeepTime = 5.0f;
	resultScoreDisappearTime = 8.0f;

	for (int i = 0; i < 500; i++)
	{
		cloudsX[i] = rand() % Define::WIN_W*2;
		cloudsY[i] = rand() % Define::WIN_H / 3;

		rcg[i] = rand() % 100;
		rcb[i] = rand() % 100;
	}
}

void GameManager::update()
{
	playingGame();
	if (ILoopController::GetInstance().GetIsPause())return;

	cloudOffset += FPS.GetFrameSecondTime();

	//========================================================================================================
	// オープニング
	//========================================================================================================
	if (_scoreModeState == ScoreModeState_Opening)//最初はプレイヤーを操作できず、オープニングが流れる
	{
		if (_openingState == OpeningState_GameGuide_Delay)//オープニングの空白時間
		{
			//DWRITE.DrawFormatText(L"OpeningDelay", Define::WIN_W / 2 - 400, 100, 1000, 100, D3D.GetColor(255, 255, 255), 1, 3);
			/*
			_gmTimer += FPS.GetFrameSecondTime();
			if (_gmTimer > openingGuideDelayTime)
			{
				_seEmitter->Play(_openingLandSE);
				_gmTimer = 0.0f;
				CameraShaker::GetInstance().GenerateShake(Shake_Random, 1, 30);
				_openingState = OpeningState_GameGuide_Appear;
			}
			*/

			//プレイヤーが着地した瞬間に変更
		}
		else if (_openingState == OpeningState_GameGuide_Land)
		{
			_seEmitter->Play(_openingLandSE);
			_gmTimer = 0.0f;
			CameraShaker::GetInstance().GenerateShake(Shake_Random, 1, 30);
			_openingState = OpeningState_GameGuide_Appear;
		}
		else if (_openingState == OpeningState_GameGuide_Appear)//オープニングのUIを出し始める
		{
			DWRITE.DrawFormatText(L"たくさんの敵を倒そう！", Define::WIN_W / 2 - 400, 100, 1000, 100, D3D.GetColor(255, 255, 255), _gmTimer / openingGuideAppearTime, 3);

			_gmTimer += FPS.GetFrameSecondTime();

			if (_gmTimer > openingGuideAppearTime)
			{
				_gmTimer = 0.0f;
				_openingState = OpeningState_GameGuide_Keep;
			}
		}
		else if (_openingState == OpeningState_GameGuide_Keep)
		{
			DWRITE.DrawFormatText(L"たくさんの敵を倒そう！", Define::WIN_W / 2 - 400, 100, 1000, 100, D3D.GetColor(255, 255, 255), 1, 3);

			_gmTimer += FPS.GetFrameSecondTime();
			_openingSEEmitter->SetMaxVolume(1 - _gmTimer / openingGuideKeepTime);
			if (_gmTimer > openingGuideKeepTime)
			{
				_openingSEEmitter->Stop();
				_openingSEEmitter->SetMaxVolume(1);

				_gmTimer = 0.0f;
				_openingState = OpeningState_GameGuide_Disappear;
			}
		}
		else if (_openingState == OpeningState_GameGuide_Disappear)
		{
			DWRITE.DrawFormatText(L"たくさんの敵を倒そう！", Define::WIN_W / 2 - 400, 100, 1000, 100, D3D.GetColor(255, 255, 255), 1 - _gmTimer / openingGuideDisappearTime, 3);

			_gmTimer += FPS.GetFrameSecondTime();
			if (_gmTimer > openingGuideDisappearTime)
			{
				_gmTimer = 0.0f;
				_weatherState = WeatherState_Sunny;
				_scoreModeState = ScoreModeState_WeatherChanging;
				_weatherChangeState = WeatherChangeState_WeatherGuide_Appear;
			}
		}
	}
	//========================================================================================================
	// 天気の変更
	//========================================================================================================
	else if (_scoreModeState == ScoreModeState_WeatherChanging)//天気を変える
	{
		if (_weatherChangeState == WeatherChangeState_WeatherGuide_Appear)
		{
			D3D.SetLayer(Layer_UI_0);
			D3D.SetAlignmentBlendDesc(_gmTimer / weatherChangeGuideAppearTime * 255);
			D3D.DrawRotImage(_climateChangeTex, Define::WIN_W / 2, Define::WIN_H / 2-100, 543 * 2, 100 * 2, 0, 1);
			D3D.DrawRotImage(_climateChange_arrowTex, Define::WIN_W / 2, Define::WIN_H / 2 + 150, 200, 100, 0, 1);
			D3D.DrawDivImage(_weatherIcon, Define::WIN_W /2+150, Define::WIN_H / 2+ 50, 200, 200, _weatherState * 96, 0, 96, 96);
			D3D.DrawDivImage(_weatherIcon, Define::WIN_W /2-350, Define::WIN_H / 2+ 50, 200, 200, _preWeatherState * 96, 0, 96, 96);
			D3D.SetDefaultBlendDesc(255);
			D3D.SetLayer(Layer_0);

			_gmTimer += FPS.GetFrameSecondTime();
			if (_gmTimer > weatherChangeGuideAppearTime)
			{
				_gmTimer = 0.0f;
				_weatherChangeState = WeatherChangeState_WeatherGuide_Keep;

				//次の天気を決める
				_preWeatherState = _weatherState;//前回の天気を保存
				int nextWeather = rd() % WeatherState_Max;
				_weatherState = (eWeatherState)nextWeather;

				//天気を変えるように各モジュールへ伝達
				if (isFirstEntry)
				{
					_weathers[_weatherState]->SetIn();
					isFirstEntry = false;
				}
				else if ((int)_preWeatherState != (int)_weatherState)//前回と違うならば
				{
					_weathers[_preWeatherState]->SetOut();
					_weathers[_weatherState]->SetIn();
					
				}
				
			}
		}
		else if (_weatherChangeState == WeatherChangeState_WeatherGuide_Keep)
		{
			D3D.SetLayer(Layer_UI_0);
			D3D.SetAlignmentBlendDesc(255);
			D3D.DrawRotImage(_climateChangeTex, Define::WIN_W / 2, Define::WIN_H / 2-100, 543 * 2, 100 * 2, 0, 1);
			D3D.DrawRotImage(_climateChange_arrowTex, Define::WIN_W / 2, Define::WIN_H / 2 +150, 200,100, 0, 1);
			D3D.DrawDivImage(_weatherIcon, Define::WIN_W / 2 + 150, Define::WIN_H / 2 + 50, 200, 200, _weatherState * 96, 0, 96, 96);
			D3D.DrawDivImage(_weatherIcon, Define::WIN_W / 2 - 350, Define::WIN_H / 2 + 50, 200, 200, _preWeatherState * 96, 0, 96, 96);
			D3D.SetDefaultBlendDesc(255);
			D3D.SetLayer(Layer_0);


			_gmTimer += FPS.GetFrameSecondTime();
			_bgmEmitter->SetMaxVolume(1 - _gmTimer / weatherChangeGuideKeepTime);
			if (_gmTimer > weatherChangeGuideKeepTime)
			{
				_bgmEmitter->Stop();
				_bgmEmitter->SetMaxVolume(1);
				_seEmitter->Play(_weatherChangeSE);
				_gmTimer = 0.0f;
				_weatherChangeState = WeatherChangeState_WeatherGuide_Disappear;
			}
		}
		else if (_weatherChangeState == WeatherChangeState_WeatherGuide_Disappear)
		{
			D3D.SetLayer(Layer_UI_0);
			D3D.SetAlignmentBlendDesc((1 - _gmTimer / weatherChangeGuideAppearTime) * 255);
			D3D.DrawRotImage(_climateChangeTex, Define::WIN_W / 2, Define::WIN_H / 2-100, 543 * 2, 100 * 2, 0, 1);
			D3D.DrawRotImage(_climateChange_arrowTex, Define::WIN_W / 2, Define::WIN_H / 2 + 150,200,100, 0, 1);
			D3D.DrawDivImage(_weatherIcon, Define::WIN_W / 2 + 150, Define::WIN_H / 2 + 50, 200, 200, _weatherState * 96, 0, 96, 96);
			D3D.DrawDivImage(_weatherIcon, Define::WIN_W / 2 - 350, Define::WIN_H / 2 + 50, 200, 200, _preWeatherState * 96, 0, 96, 96);
			D3D.SetDefaultBlendDesc(255);
			D3D.SetLayer(Layer_0);

			_gmTimer += FPS.GetFrameSecondTime();
			if (_gmTimer > weatherChangeGuideDisappearTime)
			{
				_gmTimer = 0.0f;
				_scoreModeState = ScoreModeState_WaveStart;
				_waveState = WaveStete_WaveCount_Appear;

				//次に次のウェーブのウェーブ数とそのウェーブでのノルマが出るのでそのためにここで計算する
				score.waveNum++;
				destQuota = score.waveNum * 3 + score.waveNum;
				waveTimer = 60;//1ウェーブは60秒

			}
		}
	}
	//========================================================================================================
	// 次のウェーブのノルマ情報など表示
	//========================================================================================================
	else if (_scoreModeState == ScoreModeState_WaveStart)//ウェーブスタートの表示
	{
		WCHAR wave[256];
		WCHAR waveInfo[256];
		swprintf(wave, 256, L"   WAVE %d", score.waveNum);
		swprintf(waveInfo, 256, L"  Time %d\n  ノルマ　%d 体",(int)waveTimer, destQuota);
		if (_waveState == WaveStete_WaveCount_Appear)
		{
			DWRITE.DrawFormatText(wave, Define::WIN_W / 2 - 700, 250, 2000, 100, D3D.GetColor(255, 255, 255), _gmTimer / weatherChangeGuideAppearTime, 4);
			DWRITE.DrawFormatText(waveInfo, Define::WIN_W / 2 - 400, 500, 1000, 100, D3D.GetColor(255, 255, 255), _gmTimer / weatherChangeGuideAppearTime, 3);
			_gmTimer += FPS.GetFrameSecondTime();
			if (_gmTimer > weatherChangeGuideAppearTime)
			{
				_gmTimer = 0.0f;
				_waveState = WaveStete_WaveCount_Keep;
			}
		}
		else if (_waveState == WaveStete_WaveCount_Keep)
		{
			DWRITE.DrawFormatText(wave, Define::WIN_W / 2 - 700, 250, 2000, 100, D3D.GetColor(255, 255, 255), 1, 4);
			DWRITE.DrawFormatText(waveInfo, Define::WIN_W / 2 - 400, 500, 1000, 100, D3D.GetColor(255, 255, 255), 1, 3);
			_gmTimer += FPS.GetFrameSecondTime();
			if (_gmTimer > weatherChangeGuideKeepTime)
			{
				_gmTimer = 0.0f;
				_waveState = WaveStete_WaveCount_Disappear;
			}
		}
		else if (_waveState == WaveStete_WaveCount_Disappear)
		{
			DWRITE.DrawFormatText(wave, Define::WIN_W / 2 - 700, 250, 2000, 100, D3D.GetColor(255, 255, 255), 1-_gmTimer / weatherChangeGuideDisappearTime, 4);
			DWRITE.DrawFormatText(waveInfo, Define::WIN_W / 2 - 400, 500, 1000, 100, D3D.GetColor(255, 255, 255),1- _gmTimer / weatherChangeGuideDisappearTime, 3);
			_gmTimer += FPS.GetFrameSecondTime();
			if (_gmTimer > weatherChangeGuideDisappearTime)
			{
				_gmTimer = 0.0f;
				//今回流すBGMを決める
				if (_weatherState == WeatherState_Sunny)
				{
					int index = rd() % 3;//３つの中から選ぶ
					_bgmEmitter->Play(_sunnyBgm[index]);
				}
				else if (_weatherState == WeatherState_Rainy)
				{
					int index = rd() % 3;//３つの中から選ぶ
					_bgmEmitter->Play(_rainyBgm[index]);
				}
				else if (_weatherState == WeatherState_Cloudy)
				{
					_bgmEmitter->Play(_sunnyBgm[1]);
				}
				currentQuotaValue = 0;//ノルマはリセットされる
				_scoreModeState = ScoreModeState_WaveRounding;

			}
		}
	}

	//========================================================================================================
	// ウェーブ中
	//========================================================================================================
	else if (_scoreModeState == ScoreModeState_WaveRounding)
	{
		WCHAR wave[256];
		swprintf(wave, 256, L"%d", score.waveNum);
		DWRITE.DrawFormatText(wave, 380, 20, 1000, 100, D3D.GetColor(255, 255, 255), 1, 3);

		WCHAR timer[256];
		swprintf(timer, 256, L"%d", (int)waveTimer);
		DWRITE.DrawFormatText(timer, 160, 140, 1000, 100, D3D.GetColor(255, 255, 255), 1, 2);

		WCHAR quota[256];
		swprintf(quota, 256, L"%d/%d", currentQuotaValue, destQuota);
		DWRITE.DrawFormatText(quota, 20, 300, 1000, 100, D3D.GetColor(255, 255, 255), 1, 2);

		D3D.SetAlignmentBlendDesc(200);
		D3D.SetLayer(Layer_UI_0);
		D3D.DrawImage(_scoreBoard, 0, 0, 256 * 2, 200 * 2);
		D3D.DrawRotImage(_weatherBoard, Define::WIN_W - 100, 100, 256 * 2, 256 * 2, 0, 1);
		D3D.DrawDivImage(_weatherIcon, Define::WIN_W - 256, 0, 256, 256, _weatherState * 96, 0, 96, 96);

		D3D.SetLayer(Layer_0);
		D3D.SetDefaultBlendDesc(255);

		waveTimer -= FPS.GetFrameSecondTime();
		if (waveTimer < 0)
		{
			if (currentQuotaValue >= destQuota)//ノルマを達成したら
			{
				CameraShaker::GetInstance().GenerateShake(Shake_Random, 1, 10);//演出としてカメラを揺らし、
				_enemyEraser->SetActivate(true);//敵を全部殺す
				_seEmitter->Play(_waveClearSE);
				_scoreModeState = ScoreModeState_WaveClear;//ウェーブクリア画面へ
				_clearState = ClearState_ClearGuide_Appear;

			}
			else
			{
				swprintf(gameOverStr, 256, L"時間切れ！");
				_scoreModeState = ScoreModeState_GameOver;//ウェーブクリア画面へ
				_gameoverState = GameOverState_Guide_Delay;
			}
		}


		spawnPer15sTimer += FPS.GetFrameSecondTime();
		spawnPer5sTimer += FPS.GetFrameSecondTime();

		int leastNeedEnemy = destQuota;//最低限この数を出さないとウェーブクリアできないので絶対にこの数以上は出す
		//敵が出るタイミングは、5秒に一回の１２回、１５秒に一回の４回合計１６回である

		//敵の生成タイミングを決める
		if (spawnPer5sTimer > 5)//時間が５の倍数ならば
		{
			//スポナーから数体選び生成する
			int summonNum = 0;
			int index = 0;

			//真ん中のスポナーからは必ず一体出るようにする
			summonNum = rand() % _middleSpawners.size() + 1;//１体は絶対に出る
			for (int i = 0; i < summonNum; i++)
			{
				index = rand() % _middleSpawners.size();//どのスポナーから出るか
				_middleSpawners[index]->Generate();
			}

			//横と天井からどれだけ出るか
			summonNum = rand() % ((leastNeedEnemy / 16) + 1);
			for (int i = 0; i < summonNum; i++)
			{
				//横
				index = rand() % _sideSpawners.size();
				_sideSpawners[index]->Generate();
			}

			summonNum = rand() % ((leastNeedEnemy / 16) + 1);
			for (int i = 0; i < summonNum; i++)
			{
				//上下
				index = rand() % _upDownSpawners.size();
				_upDownSpawners[index]->Generate();
			}

			spawnPer5sTimer = 0.0f;
		}

		if (spawnPer15sTimer > 15)//時間が１５の倍数ならば
		{
			//スポナーから数体選び生成する
			int summonNum = rand() % 10;
			for (int i = 0; i < summonNum; i++)
			{
				int index = rand() % _spawner.size();
				_spawner[index].get()->Generate();
			}
			spawnPer15sTimer = 0.0f;
		}


		DWRITE.DrawFormatText(L"Weather", Define::WIN_W - 600, 10, 1000, 100, D3D.GetColor(255, 255, 255), 1, 3);
	}
	//========================================================================================================
	// ウェーブクリア
	//========================================================================================================
	else if (_scoreModeState == ScoreModeState_WaveClear)
	{
		WCHAR quota[256];
		swprintf(quota, 256, L"%d", currentQuotaValue);
		if (_clearState == ClearState_ClearGuide_Appear)
		{

			D3D.SetLayer(Layer_UI_0);
			D3D.SetAlignmentBlendDesc((_gmTimer / clearGuideAppearTime) * 255);
			D3D.DrawRotImage(_clearTex, Define::WIN_W / 2, Define::WIN_H / 2, 430 * 2, 174 * 2, 0, 1);
			DWRITE.DrawFormatText(quota, Define::WIN_W / 2, Define::WIN_H / 2+50, 1000, 100, D3D.GetColor(255, 255, 255), 1, 3);
			D3D.SetDefaultBlendDesc(255);
			D3D.SetLayer(Layer_0);

			_gmTimer += FPS.GetFrameSecondTime();
			if (_gmTimer > clearGuideAppearTime)
			{
				_enemyEraser->SetActivate(false);
				_gmTimer = 0.0f;
				_clearState = ClearState_ClearGuide_Keep;
			}
		}
		else if (_clearState == ClearState_ClearGuide_Keep)
		{
			D3D.SetLayer(Layer_UI_0);
			D3D.SetAlignmentBlendDesc(255);
			D3D.DrawRotImage(_clearTex, Define::WIN_W / 2, Define::WIN_H / 2, 430 * 2, 174 * 2, 0, 1);
			DWRITE.DrawFormatText(quota, Define::WIN_W / 2, Define::WIN_H / 2 + 50, 1000, 100, D3D.GetColor(255, 255, 255), 1, 3);
			D3D.SetDefaultBlendDesc(255);
			D3D.SetLayer(Layer_0);
			_gmTimer += FPS.GetFrameSecondTime();

			if (_gmTimer > clearGuideKeepTime)
			{
				_gmTimer = 0.0f;
				_clearState = ClearState_ClearGuide_Disappear;
			}
		}
		else if (_clearState == ClearState_ClearGuide_Disappear)
		{
			D3D.SetLayer(Layer_UI_0);
			D3D.SetAlignmentBlendDesc((1 - _gmTimer / clearGuideAppearTime) * 255);
			D3D.DrawRotImage(_clearTex, Define::WIN_W / 2, Define::WIN_H / 2, 430 * 2, 174 * 2, 0, 1);
			DWRITE.DrawFormatText(quota, Define::WIN_W / 2, Define::WIN_H / 2 + 50, 1000, 100, D3D.GetColor(255, 255, 255), 1, 3);
			D3D.SetDefaultBlendDesc(255);
			D3D.SetLayer(Layer_0);
			_gmTimer += FPS.GetFrameSecondTime();
			if (_gmTimer > clearGuideDisappearTime)
			{
				_gmTimer = 0.0f;
				_scoreModeState = ScoreModeState_WeatherChanging;
				_weatherChangeState = WeatherChangeState_WeatherGuide_Appear;

			}
		}
	}

	//========================================================================================================
	// ゲームオーバー
	//========================================================================================================
	else if (_scoreModeState == ScoreModeState_GameOver)
	{
		if (_gameoverState == GameOverState_Guide_Delay)
		{
			_gmTimer += FPS.GetFrameSecondTime();
			_bgmEmitter->SetMaxVolume(1 - _gmTimer / gameOverGuideDelayTime);
			if (_gmTimer > gameOverGuideDelayTime)
			{
				_bgmEmitter->Stop();
				_bgmEmitter->SetMaxVolume(1);
				_gmTimer = 0.0f;
				_gameoverState = GameOverState_Guide_Appear;
			}
		}
		if (_gameoverState == GameOverState_Guide_Appear)
		{
			DWRITE.DrawFormatText(gameOverStr, Define::WIN_W / 2 - 300, 500, 1000, 100, D3D.GetColor(255, 255, 255), _gmTimer / gameOverGuideAppearTime, 3);
			_gmTimer += FPS.GetFrameSecondTime();
			if (_gmTimer > gameOverGuideAppearTime)
			{
				_gmTimer = 0.0f;
				_gameoverState = GameOverState_Guide_Keep;
			}
		}
		else if (_gameoverState == GameOverState_Guide_Keep)
		{
			DWRITE.DrawFormatText(gameOverStr, Define::WIN_W / 2 - 300, 500, 1000, 100, D3D.GetColor(255, 255, 255), 1, 3);
			_gmTimer += FPS.GetFrameSecondTime();
			if (_gmTimer > gameOverGuideKeepTime)
			{
				_gmTimer = 0.0f;
				_gameoverState = GameOverState_Guide_Disappear;
			}
		}
		else if (_gameoverState == GameOverState_Guide_Disappear)
		{
			_seEmitter->SetMaxVolume(1 - _gmTimer / gameOverGuideDisappearTime);
			DWRITE.DrawFormatText(gameOverStr, Define::WIN_W / 2 - 300, 500, 1000, 100, D3D.GetColor(255, 255, 255), 1 - _gmTimer / gameOverGuideDisappearTime, 3);
			_gmTimer += FPS.GetFrameSecondTime();
			if (_gmTimer > gameOverGuideDisappearTime)
			{
				_seEmitter->Stop();
				_seEmitter->SetMaxVolume(1);
				_gmTimer = 0.0f;
				_scoreModeState = ScoreModeState_Result;//リザルト画面に移る
				_resultState = ResultState_Guide_Appear;
				//DWRITE.DrawFormatText(L"タイトルに戻る", Define::WIN_W / 2 - 400, 500, 1000, 100, D3D.GetColor(255, 255, 255), 1, 2);
			}

			/*
			if (Pad::GetInstance().GetPadDown(ePad::jump))
			{
				_spawner.clear();
				isReturnTitle = true;
			}
			*/
		}

	}

	//========================================================================================================
	// リザルト
	//========================================================================================================
	else if (_scoreModeState == ScoreModeState_Result)
	{
		if (_resultState == ResultState_Guide_Appear)
		{
			_gmTimer += FPS.GetFrameSecondTime();
			DWRITE.DrawD2DBox(0, 0, Define::WIN_W + 100, Define::WIN_H + 100, D3D.GetColor(10, 10, 10), _gmTimer / resultScoreAppearTime, 1, 1);
			if (_gmTimer > resultScoreAppearTime)
			{
				_seEmitter->Play(_showResultSE);
				_gmTimer = 0.0f;
				_resultState = ResultState_Guide_Keep;
			}
		}
		else if (_resultState == ResultState_Guide_Keep)
		{
			DWRITE.DrawD2DBox(0, 0, Define::WIN_W + 100, Define::WIN_H + 100, D3D.GetColor(10, 10, 10), 1, 1, 1);
			WCHAR totalWave[256];
			swprintf(totalWave, 256, L"到達WAVE %d", score.waveNum);
			DWRITE.DrawFormatText(totalWave, Define::WIN_W / 2 + 200, 400, 1000, 100, D3D.GetColor(255, 255, 255), 1, 2);

			WCHAR totalDefeatEnemyNum[256];
			swprintf(totalDefeatEnemyNum, 256, L"倒した敵の数 %d", score.defeatedNum);
			DWRITE.DrawFormatText(totalDefeatEnemyNum, Define::WIN_W / 2 + 200, 500, 1000, 100, D3D.GetColor(255, 255, 255), 1, 2);

			WCHAR totalEnemyDamage[256];
			swprintf(totalEnemyDamage, 256, L"敵に与えたダメージ %d", score.enemyTotalDamage);
			DWRITE.DrawFormatText(totalEnemyDamage, Define::WIN_W / 2 + 200, 600, 1000, 100, D3D.GetColor(255, 255, 255), 1, 2);

			WCHAR rank[256];
			WCHAR flavorText[256];
			if (score.waveNum==0)
			{
				swprintf(rank, 256, L"？");
				
			}
			else if (score.waveNum > 0 && score.waveNum < 3)
			{
				swprintf(rank, 256, L"D");
				
			}
			else if (score.waveNum >= 3 && score.waveNum < 6)
			{
				swprintf(rank, 256, L"C");
				

			}
			else if (score.waveNum >= 6 && score.waveNum < 9)
			{
				swprintf(rank, 256, L"B");
				

			}
			else if (score.waveNum >= 9 && score.waveNum < 12)
			{
				swprintf(rank, 256, L"A");
				

			}
			else if (score.waveNum >= 12)
			{
				swprintf(rank, 256, L"S");
				

			}
			DWRITE.DrawFormatText(L"Rank", Define::WIN_W / 2 - 500, 300, 1000, 100, D3D.GetColor(255, 255, 255), 1, 3);
			DWRITE.DrawFormatText(rank, Define::WIN_W / 2 - 500, 400, 1000, 100, D3D.GetColor(255, 255, 255), 1, 4);
		
			_gmTimer += FPS.GetFrameSecondTime();
			if (_gmTimer > resultScoreKeepTime)
			{

				DWRITE.DrawFormatText(L"SAPCE タイトルに戻る", Define::WIN_W / 2 - 100, 900, 1000, 100, D3D.GetColor(255, 255, 255), 1, 2);

				if (Keyboard::GetInstance().GetKeyDown(DIK_NUMPADENTER)|| Keyboard::GetInstance().GetKeyDown(DIK_SPACE)||Pad::GetInstance().GetPadDown(ePad::jump))
				{
					_bgmEmitter->Stop();
					_gmTimer = 0.0f;
					isReturnTitle = true;
					finalize();
				}
			}
		}
		
	}

}

void GameManager::finalize()
{
	_spawner.clear();
	_middleSpawners.clear();
	_sideSpawners.clear();
	_upDownSpawners.clear();
	_weathers.clear();

}

void GameManager::playingGame()//ゲーム中のステートによらない更新はここで行う
{


	cameraPallaxPos = CAMERA.GetCameraPos();

	XMFLOAT2 diff = cameraPallaxPos - preCameraPallaxPos;
	backGroundPos_Layer5 += diff;
	D3D.SetDefaultBlendDesc(255);
	D3D.SetLayer(Layer_5);
	D3D.DrawImage(_sunnyTex, backGroundPos_Layer5.x, backGroundPos_Layer5.y, 4000, 4000);
	D3D.SetLayer(Layer_0);
	D3D.SetAlignmentBlendDesc(255);
	backGroundPos_Layer_Cloud -= diff * 0.5f;
	for (int i = 0; i < 200; i++)
	{
		float offsetX = sin((float)(102454 * i)) * 3000;
		float offsetY = sin((float)(13224354 * i)) * 1500;
		float offsetSizeX = sin((float)(235454 * i)) * 800;
		float offsetSizeY = sin((float)(123554 * i)) * 500;
		D3D.DrawImage(_cloudTex, backGroundPos_Layer_Cloud.x + 1000 + offsetX, backGroundPos_Layer_Cloud.y - 1200 + offsetY, 600 + offsetSizeX, 600 + offsetSizeX);
	}
	D3D.SetDefaultBlendDesc(255);

	D3D.SetAlignmentBlendDesc(255);
	backGroundPos_Layer4 += diff * 0.8f;
	for (int i = 0; i < 250; i++)
	{
		D3D.SetLayer(Layer_0);
		D3D.DrawRotImage(_cloudTex, backGroundPos_Layer4.x+(int)(cloudsX[i] + (cloudOffset * cloudsY[i])) % (6000) - 3000, 
			backGroundPos_Layer4.y+cloudsY[i] + 1700 + sin(cloudsX[i] + cloudOffset) * 20
			, 20, 20, (float)cloudsX[i] / 20, (float)cloudsY[i] / 15 + 0.3f);
	}

	for (int i = 250; i < 500; i++)
	{
		D3D.SetLayer(Layer_0);
		D3D.DrawRotImage(_cloudTex, (int)(cloudsX[i] + (cloudOffset * cloudsY[i])) % (6000) - 2500, cloudsY[i] + 1800 + sin(cloudsX[i] + cloudOffset) * 20
			, 20, 20, (float)cloudsX[i] / 20, (float)cloudsY[i] / 10 + 0.3f);
	}
	D3D.SetDefaultBlendDesc(255);


	if (Keyboard::GetInstance().GetKeyDown(DIK_ESCAPE)||Pad::GetInstance().GetPadDown(pause))
	{
		if (!isPause)
		{
			isPause = true;
			ILoopController::GetInstance().Pause(true);
			D3D.getDiskBlurFilter()->setIsApply(true);
			D3D.getDiskBlurFilter()->setScale(1);
			D3D.SetIsDrawLayer(false, eLayer::Layer_UI_0);
			D3D.SetIsDrawLayer(false, eLayer::Layer_UI_Effect);
			D3D.SetIsDrawLayer(false, eLayer::Layer_UI_2);
			D3D.SetIsDrawLayer(false, eLayer::Layer_WorldUI);
			_openingSEEmitter->Pause();
			_bgmEmitter->Pause();
			_seEmitter->Pause();
		}
		else
		{
			isPause = false;
			ILoopController::GetInstance().Pause(false);
			D3D.getDiskBlurFilter()->setIsApply(false);
			D3D.getDiskBlurFilter()->setScale(0);
			D3D.SetIsDrawLayer(true, eLayer::Layer_UI_0);
			D3D.SetIsDrawLayer(true, eLayer::Layer_UI_Effect);
			D3D.SetIsDrawLayer(true, eLayer::Layer_UI_2);
			D3D.SetIsDrawLayer(true, eLayer::Layer_WorldUI);
			_openingSEEmitter->Resume();

			_bgmEmitter->Resume();
			_seEmitter->Resume();

		}
	}
	preCameraPallaxPos = cameraPallaxPos;
}

void GameManager::draw()const
{

}

