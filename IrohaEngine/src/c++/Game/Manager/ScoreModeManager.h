#pragma once
#include <Iroha.h>

//======================================================================
// スコアアタックモードのゲームルール
//======================================================================
//　プレイヤーはステージ外に落下するか、HPがゼロになるとゲームオーバーとする
//　ウエーブ数をどれだけこなせるかで競う
//  
//

enum eScoreModeState
{
	ScoreModeState_Opening,//オープニング
	ScoreModeState_WeatherChanging,//天候変更中
	ScoreModeState_WaveRounding,//ウェーブの最中
	ScoreModeState_WaveClear,
	ScoreModeState_GameOver,//ゲームオーバー
};

//スコアモードマネージャークラス：ゲームの進行をするクラス。ステートの変化によってゲームを進行させる
class ScoreModeManager :public Singleton<ScoreModeManager>
{
public:



	ScoreModeManager()
	{

	}
	virtual ~ScoreModeManager()
	{

	}
	void Init();
	void Update();
	void Unload();

private:

	UINT waveCount = 0;//現在のウェーブ数
	UINT totalDamageDealt = 0;//与えたダメージの総量
	UINT totalDamageRecieved = 0;//受けたダメージの総量
	UINT rank = 0;//現在のランク
	UINT atmosphereScore = 0;//ベンチマークスコア

	//==============================================
	// 演出用時間変数
	//==============================================

	float openingTimer = 0.0f;//オープニング時間
	float weatherChangingTimer = 0.0f;//天候変更の時間
};

