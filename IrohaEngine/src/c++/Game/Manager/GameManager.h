#pragma once
#include <Iroha.h>

#include "General/AmbientSoundEmitter.h"
#include "Gimmick/DeathZone.h"
#include "Module/Climate/Atmosphere.h"
#include "General/SceneBGMEmitter.h"
#include "General/SceneSEEmitter.h"
#include "Gimmick/Spawner/EnemySpawner.h"
#include "Gimmick/Spawner/ItemSpawner.h"
#include "Gimmick/EnemyEraser.h"

#include "Module/Climate/Sunny.h"
#include "Module/Climate/Cloudy.h"
#include "Module/Climate/Rainy.h"

//ゲームのルール
//プレイヤーはステージ外に落下するか、3回ダメージを受けるとゲームオーバー
//プレイヤーがゴールに触れるとステージクリアとする
//


enum eWeatherState
{
	WeatherState_Sunny,
	WeatherState_Rainy,
	WeatherState_Cloudy,
	WeatherState_Max,

};

enum eScoreModeState
{
	ScoreModeState_Opening,//オープニング
	ScoreModeState_WeatherChanging,//天候変更中
	ScoreModeState_WaveStart,//天候変更中
	ScoreModeState_WaveRounding,//ウェーブの最中
	ScoreModeState_WaveClear,//ウェーブクリア
	ScoreModeState_GameOver,//ゲームオーバー
	ScoreModeState_Result//リザルト
};

enum eOpeningState
{
	OpeningState_GameGuide_Delay,
	OpeningState_GameGuide_Land,
	OpeningState_GameGuide_Appear,
	OpeningState_GameGuide_Keep,
	OpeningState_GameGuide_Disappear,
};

enum eWeatherChangeState
{
	WeatherChangeState_WeatherGuide_Appear,
	WeatherChangeState_WeatherGuide_Keep,
	WeatherChangeState_WeatherGuide_Disappear,
};

enum eWaveState
{
	WaveStete_WaveCount_Appear,
	WaveStete_WaveCount_Keep,
	WaveStete_WaveCount_Disappear,
};

enum eClearState
{
	ClearState_ClearGuide_Appear,
	ClearState_ClearGuide_Keep,
	ClearState_ClearGuide_Disappear,

	ClearState_ScoreGuide_Appear,
	ClearState_ScoreGuide_Keep,
	ClearState_ScoreGuide_Disappear,
};

enum eGameOverState
{
	GameOverState_Guide_Delay,//カメラがプレイヤーによって行く
	GameOverState_Guide_Appear,
	GameOverState_Guide_Keep,
	GameOverState_Guide_Disappear,
};

enum eResultState
{
	ResultState_Guide_Appear,
	ResultState_Guide_Keep,//ここで順に出てくる
	ResultState_Guide_Disappear,
};

//ゲームマネージャークラス：ゲームの進行をするクラス。ステートの変化によってゲームを進行させる
class GameManager :public Singleton<GameManager>
{


public:
	GameManager();
	virtual ~GameManager();
	void init();
	void update();
	void draw()const;
	bool unload();
	void finalize();
	bool IsGameEnd()
	{
		if (isReturnTitle)return true;
		else return false;
	}

	void SetState(eScoreModeState state)
	{
		_scoreModeState = state;
		if (state == eScoreModeState::ScoreModeState_GameOver)
		{
			_gameoverState = eGameOverState::GameOverState_Guide_Delay;
		}
	}

	void SetOpeningState(eOpeningState state)
	{
		_openingState = state;
	}

	eScoreModeState GetState()const
	{
		return _scoreModeState;
	}

	eOpeningState GetOpeningState()
	{
		return _openingState;
	}

	void AddDefeatedNum()
	{
		score.defeatedNum++;
		currentQuotaValue++;//ノルマも増やす
	}

	void AddTotalEnemyDamage(float damage)
	{
		score.enemyTotalDamage += damage;
	}

	void AddTotalPlayerDamage(float damage)
	{
		score.playerTotalDamage += damage;
	}

private:

	
	void playingGame();

	float stateTimer = 0.0f;
	bool isPause = false;
	bool isEnemySpawnerActivated = false;
	bool isReturnTitle = false;
	bool isFirstEntry = true;

	eWeatherState _weatherState = eWeatherState::WeatherState_Sunny;
	eWeatherState _preWeatherState = eWeatherState::WeatherState_Sunny;
	eScoreModeState _scoreModeState = eScoreModeState::ScoreModeState_Opening;
	eOpeningState _openingState = eOpeningState::OpeningState_GameGuide_Appear;
	eWeatherChangeState _weatherChangeState = eWeatherChangeState::WeatherChangeState_WeatherGuide_Appear;
	eWaveState _waveState = eWaveState::WaveStete_WaveCount_Appear;
	eClearState _clearState = eClearState::ClearState_ClearGuide_Appear;
	eGameOverState _gameoverState = eGameOverState::GameOverState_Guide_Appear;
	eResultState _resultState = eResultState::ResultState_Guide_Appear;
	//画像
	int _sunnyTex = 0;
	int _verySunnyTex = 0;
	int _leafTex = 0;
	int _cloudTex = 0;
	int _scoreBoard = 0;
	int _weatherBoard = 0;
	int _weatherIcon = 0;

	int _climateChangeTex = 0;
	int _climateChange_arrowTex = 0;
	int _tillendTex = 0;
	int _clearTex = 0;

	//音声
	int _sunnyBgm[3];
	int _rainyBgm[3];
	int _cloudyBgm = 0;

	int _openingFallSE = 0;
	int _openingLandSE = 0;
	int _waveClearSE = 0;
	int _weatherChangeSE = 0;
	int _showResultSE = 0;
	int _gameOverSE = 0;//ゲームオーバーになるときの音

	//雲の描画用変数
	float cloudOffset = 0;
	int cloudsX[500];
	int cloudsY[500];
	int rcg[500];
	int rcb[500];

	//=============================================================
	// リザルトに使用するスコア
	//=============================================================
	struct resultScore
	{

		UINT waveNum;
		UINT defeatedNum;
		UINT playerTotalDamage;
		UINT enemyTotalDamage;
		UINT rank;
	};

	UINT currentQuotaValue = 0;//１ウェーブ当たりのノルマ
	UINT destQuota = 0;//１ウェーブ当たりのノルマ
	resultScore score;
	float waveTimer = 0.0f;

	float spawnPer5sTimer = 0;
	float spawnPer15sTimer = 0;
	int generateOffsetRatePer1 = 3;//敵を出す割合（５ウェーブごとに上昇し、生成量に影響する）
	int generateOffsetRatePer5 = 3;//敵を出す割合（５ウェーブごとに上昇し、生成量に影響する）

	//=============================================================
	// UIなどのステート管理に用いる変数
	//=============================================================

	//ゲーム内容のガイドが出てくる時間
	float openingGuideDelayTime = 10.0f;
	float openingGuideAppearTime = 0.4f;
	float openingGuideKeepTime = 3.0f;//ガイドがとどまる時間
	float openingGuideDisappearTime = 0.5f;//ガイドが消える時間

	//ウェーブの案内が出てくる時間(今何ウェーブめか）
	float openingWaveCountAppearTime = 0.4f;
	float openingWaveCountKeepTime = 0.4f;//とどまる時間
	float openingWaveCountDisappearTime = 0.4f;//消える時間

	//天気変更の案内が出てくる時間
	float weatherChangeGuideAppearTime = 0.3f;
	float weatherChangeGuideKeepTime = 4.0f;
	float weatherChangeGuideDisappearTime = 0.3f;

	//クリアした時のガイド
	float clearGuideAppearTime = 0.4f;
	float clearGuideKeepTime = 3;
	float clearGuideDisappearTime = 0.4f;

	//クリアした時のスコアの表示
	float clearScoreCountAppearTime = 0.4f;
	float clearScoreCountKeepTime = 3;
	float clearScoreCountDisappearTime = 0.4f;

	//ゲームオーバー時のガイド
	float gameOverGuideDelayTime=2;
	float gameOverGuideAppearTime = 0.4f;
	float gameOverGuideKeepTime = 3.0f;
	float gameOverGuideDisappearTime = 0.4f;

	//ゲームオーバーになったときのリザルト
	float resultScoreAppearTime = 0.4f;
	float resultScoreKeepTime = 3.0f;
	float resultScoreDisappearTime = 0.4f;

	float _gmTimer = 0;//ゲームマネージャーのタイマー

	XMFLOAT2 cameraPallaxPos = { 0,0 };
	XMFLOAT2 preCameraPallaxPos = { 0,0 };

	XMFLOAT2 backGroundPos_Layer5 = { 0,0 };
	XMFLOAT2 backGroundPos_Layer4 = { 0,0 };
	XMFLOAT2 backGroundPos_Layer_Cloud = { 0,0 };

	WCHAR gameOverStr[256];

	std::shared_ptr<EnemySpawner> _leftEnemySpawner = nullptr;
	std::shared_ptr<EnemySpawner> _rightEnemySpawner = nullptr;
	std::shared_ptr<EnemySpawner> _ceilingEnemySpawner = nullptr;
	std::shared_ptr<EnemySpawner> _bottomEnemySpawner = nullptr;
	std::shared_ptr<EnemySpawner> _middleEnemySpawner_0 = nullptr;
	std::shared_ptr<EnemySpawner> _middleEnemySpawner_1 = nullptr;
	std::shared_ptr<EnemySpawner> _middleEnemySpawner_2 = nullptr;

	std::shared_ptr<ItemSpawner> _bottomItemSpawner = nullptr;
	std::shared_ptr<ItemSpawner> _ceilingItemSpawner = nullptr;

	std::vector<std::shared_ptr<ISpawner>> _spawner;
	std::vector<std::shared_ptr<EnemySpawner>> _middleSpawners;
	std::vector<std::shared_ptr<EnemySpawner>> _sideSpawners;
	std::vector<std::shared_ptr<EnemySpawner>> _upDownSpawners;
	std::vector<std::shared_ptr<IWeather>> _weathers;

	std::shared_ptr<Atmosphere> _atmosphere = nullptr;

	std::shared_ptr<DeathZone> _deathZone = nullptr;

	std::shared_ptr<SceneSEEmitter> _openingSEEmitter = nullptr;
	std::shared_ptr<SceneSEEmitter> _seEmitter = nullptr;


	std::shared_ptr<SceneBGMEmitter> _bgmEmitter = nullptr;
	std::shared_ptr<EnemyEraser> _enemyEraser = nullptr;

	std::random_device rd;
};

