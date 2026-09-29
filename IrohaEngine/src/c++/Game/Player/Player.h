#pragma once
#include <Iroha.h>

#include "../Module/Attack/CoopAttack.h"
#include "../Module/Attack/IndivAttack.h"
#include "../Module/Attack/FinalAttack.h"
#include "../Manager/GameManager.h"
#include "SubPlayer.h"
#include "UI/PlayerGuideUI.h"


class SubPlayer;
class PlayerGuideUI;
//メインプレイヤー側はステート制御を行う。サブはメインのステートに書き込める。
class Player :public Entity
{

private:

	XMFLOAT2 _v = { 0,0 };
	XMFLOAT2 _a = { 0,0 };

	int _texId = 0;
	int _finalAttackMainTex = 0;
	int _finalAttackSubTex = 0;

	float _finalAttackTexAppearTime = 0.8f;
	float _finalAttackTexKeepTime = 1;
	float _finalAttackTexDisappearTime=0.8f;

	float _finalAttackMainTexTimer = 0.0f;
	float _finalAttackSubTexTimer = 0.0f;
	XMFLOAT2 _finalAttackMainTexPos = { 400,400 };
	XMFLOAT2 _finalAttackSubTexPos = { 800,400 };

	//======================================
	// 効果音
	//======================================

	int _jumpStartSE = 0;//_ジャンプし始めの音
	int _jumpEndSE =0 ;
	int _attackActivatedAirSE = 0;//とにかく攻撃を出したら出る音(空中)
	int _attackActivatedSE = 0;//とにかく攻撃を出したら出る音

	int _finalAttackActivatedSE = 0;//必殺技が発動した時の音
	int _hpAssertSE = 0;//体力が残り少ない時の警告音
	int _flowActivatedSE = 0;//フロー状態になったときの音
	int _afterJumpGroundSE=0;//着地の音
	int _downSE = 0;//ダウンした時の音
	int _gainEPSE = 0;//EPを取得した時の音
	int _gainHPSE = 0;
	int _penaltyEPSE = 0;//連続で個人技を使った事で発生したペナルティの音
	int _damagedSE = 0;//ダメージを受けたときの音

	//======================================
	// 状態フラグ
	//======================================
	// 
	//最初の着地の演出用フラグ
	bool isFirstLand = false;
	//移動と見た目
	bool isFloatingAir = false;
	bool isJump = false;
	bool isFlip = false;

	//その行動が可能ならそのフラグはtrueになる
	bool isCoopEnabled = false;//協力攻撃できるか
	bool isMainIndivEnabled = false;//メインは個人技を使えるか
	bool isMainFinalEnabled = false;//メインは必殺技が使えるか
	bool isSubIndivEnabled = false;//サブは個人技を使えるか
	bool isSubFinalEnabled = false;//メインは必殺技が使えるか

	//気力が50%以上ならこのフラグはtrueになる
	bool isMainFlowActivated = false;//メインがフロー状態か
	bool isSubFlowActivated = false;//サブがフロー状態か

	//50%を超えた瞬間に音が鳴るようにさせるためのフラグ
	bool isMainFlowActivatedOnce = true;
	bool isSubFlowActivatedOnce = true;

	//無敵フラグ
	bool isMainInvincible = false;//メインは無敵状態か
	bool isSubInvincible = false;//サブは無敵状態か

	//スタンフラグ
	bool isMainStun = false;//メインはスタン状態か
	bool isSubStun = false;//サブはスタン状態か

	float _flyingRegist = 1.0f;//空中にいるときの加速のしにくさの係数
	float accelTimer = 0.0f;//加速に用いる変数
	float width = 64;//これが１メートル
	float height = 64;

	//======================================
	// 基礎パラメータ
	//======================================

	float defaultMaxSpeed = 5;
	float maxSpeed = 50;
	float mainHeight = 156;//センチメートル
	float subHeight = 170;

	//======================================
	// HP、EP
	//======================================
	UINT maxHP = 100;//HPの最大値
	UINT currentHP = 100;//HPは共有パラメータである

	UINT mainMaxEP = 100;//メインのEPの最大値
	UINT mainCurrentEP = 25;//メインのEP

	UINT subMaxEP = 100;//サブのEPの最大値
	UINT subCurrentEP = 25;//サブのEP

	float penaltyDisappearTime = 3.0;//連続でこの時間以内に攻撃しなければ連続回数は0にリセットされる。
	float mainPenaltyEPTimer = 0.0f;//一定時間たつとリセットされるペナルティのタイマー
	float subPenaltyEPTimer = 0.0f;//一定時間たつとリセットされるペナルティのタイマー


	//======================================
	// 攻撃に関するパラメータ
	//======================================

	//協力攻撃
	UINT coopAttackValue = 160;//攻撃力
	UINT coopAttackRange = 200;//攻撃判定の大きさ

	//個人攻撃メイン
	UINT mainIndivAttackValue = 65;
	UINT mainIndivAttackRange = 128;

	//個人攻撃サブ
	UINT subIndivAttackValue = 65;
	UINT subIndivAttackRange = 128;

	//必殺技メイン
	UINT mainFinalAttackValue = 300;
	UINT mainFinalAttackRange = 1000;

	//必殺技サブ
	UINT subFinalAttackValue = 300;
	UINT subFinalAttackRange = 1000;

	UINT finalAttackEPConsumeValue = 55;//必殺技で消費されるEP

	//======================================
	// 攻撃ステート管理用時間変数
	//======================================
	//接地時攻撃共通の攻撃に用いる時間変数
	float attackBeforeOccurrenceTime = 0.1f;//攻撃発生前の空白の時間
	float attackKeepTime = 0.3f;            //攻撃が実際に発生する時間
	float attackAfterLockTime = 0.1f;      //攻撃後に再度入力を受け付けるまでの時間

	//空中時攻撃共通の攻撃に用いる時間変数
	float attackBeforeOccurrenceTimeAir = 0.1f;//攻撃発生前の空白の時間
	float attackKeepTimeAir = 0.5f;            //攻撃が実際に発生する時間
	float attackAfterLockTimeAir = 0.3f;      //攻撃後に再度入力を受け付けるまでの時間

	//必殺技に用いる時間変数
	float finalAttackBeforeOccurrenceTime = 0.2f;
	float finalAttackKeepTime = 0.7f;
	float finalAttackAfterLockFlame = 0.5f;

	//攻撃全般に使われる時間変数
	float attackMainTimer = 0.0f;//メイン側の攻撃の時間管理用変数
	float attackSubTimer = 0.0f;//サブ側の攻撃の時間管理用変数

	//======================================
	// ヒットストップ管理用変数
	//======================================
	//ヒットストップのフラグ
	bool isMainHitStop = false;
	bool isSubHitStop = false;

	//ヒットストップ時間および大きさ
	float mainHitStopTime = 0;//外部から入力されるので設定しなくてよい（攻撃力に応じてや倒した時になどの条件で決まる）
	float mainHitStopTimer = 0;
	float mainHitStopMagnitude = 0;
	float subHitStopTime = 0;
	float subHitStopTimer = 0;
	float subHitStopMagnitude = 0;

	//ヒットストップのオフセット量
	XMFLOAT2 mainHitStopOffset = { 0,0 };
	XMFLOAT2 subHitStopOffset = { 0,0 };

	//ヒットストップ前の速度。ヒットストップ中に保存し、終わったら速度に適用する。
	XMFLOAT2 mainHitStopStoreVelocity = {0,0};
	XMFLOAT2 subHitStopStoreVelocity = {0,0};

	//======================================
	// ダウン、スタン、ステート管理用時間変数（無敵フラグと併用）
	//======================================

	float stunUncontrollableTime = 0.3f;           //スタン後の操作不能時間
	float stunInvincibleTime = 1;                  //スタン後の無敵時間
	float mainStunTimer = 0.0f;
	float subStunTimer = 0.0f;



	float mainInvincibleFlushTimer = 0.0f;//無敵時間の点滅に使われるタイマー
	float subInvincibleFlushTimer = 0.0f;
	float invincibleFlushTime = 0.03f;

	bool isMainVisible = true;
	bool isSubVisible = true;

	float mainInvincibleTimer = 0.0f;//無敵時間の計測に使われるタイマー（スタンやダウン時に設定される）０になるまでカウントされ、０になったら強制的に無敵を解除する。
	float subInvincibleTimer = 0.0f;

	//=====================================
	// ジャンプ制御用変数
	//=====================================
	float defaultJumpPower = 20.0f;
	float jumpPowerFactor = 0.0f;
	float jumpPower = 0.0f;
	float g = Define::Gravity;

	//=====================================
	// カメラ制御用変数
	//=====================================
	XMFLOAT2 jumpPos = { 0,0 };
	float cameraTimer = 0.0f;
	bool isPreJump = false;
	XMFLOAT2 cameraPos = { 0,0 };
	XMFLOAT2 cameraOpeningOffset = { 0,0 };
	XMFLOAT2 cameraVel = { 0,0 };

	float gameOverCameraDestZoomValue = 2.0f;
	float gameOverCameraZoomTime = 1.0;
	float gameOverCameraZoomTimer = 0.0f;

	//=====================================
	// 必殺技制御用変数
	//=====================================

	//必殺技の時に出す画像の位置
	XMFLOAT2 mainIconPos = { 100,100 };
	XMFLOAT2 subIconPos = { 300,100 };

	XMFLOAT2 finalIconScale = { 500,500 };

	//最初に出てくる時間
	float mainIconAppearTime = 0.2;

	//とどまる時間
	float mainIconKeepTime = 1;

	//消滅する時間
	float mainDisappearTime = 0.5;

	
	float mainIconTimer = 0.0f;
	float subIconTimer = 0.0f;

	int iu = 0;
	enum eInput_State
	{
		Input_Idle,
		Input_Up,
		Input_Down,
		Input_Right,
		Input_Left,
	};


	//全体で共有するステート
	enum eCommonState
	{
		Common_None,
		Common_Death,//死亡状態
	};

	enum eMainState
	{
		Main_Idle,   //直立
		Main_Walk,   //歩き
		Main_Dush,   //ダッシュ
		Main_Jump,   //ジャンプ
		Main_Fall,   //落下
		Main_Damaged,//ダメージくらい
		Main_Invicble//ダメージくらい後の無敵
	};

	enum eSubState
	{
		Sub_Idle,   //直立
		Sub_Walk,   //歩き
		Sub_Dush,   //ダッシュ
		Sub_Jump,   //ジャンプ
		Sub_Fall,   //落下
		Sub_Damaged,//ダメージくらい
		Sub_Invicble//ダメージくらい後の無敵
	};

	//メイン側の攻撃ステート（協力攻撃と個人技は排他的なのでここに統合している）
	enum eMainAttackState
	{
		MainAttackState_None,

		MainAttackState_CoopBeforeOccurence,   //Coopアニメーション発生
		MainAttackState_CoopBeforeOccurenceAir,//Coop空中アニメーション発生
		MainAttackState_CoopKeep,              //Coop攻撃発生
		MainAttackState_CoopKeepAir,           //Coop空中攻撃発生
		MainAttackState_CoopLock,              //Coop攻撃後硬直
		MainAttackState_CoopLockAir,           //Coop空中攻撃後硬直

		MainAttackState_IndivBeforeOccurence,   //Indivアニメーション発生
		MainAttackState_IndivBeforeOccurenceAir,//Indiv空中アニメーション発生
		MainAttackState_IndivKeep,              //Indiv攻撃発生
		MainAttackState_IndivKeepAir,           //Indiv空中攻撃発生
		MainAttackState_IndivLock,              //Indiv攻撃後硬直
		MainAttackState_IndivLockAir,           //Indiv空中攻撃後硬直

		MainAttackState_FinalBeforeOccurence,   //Finalアニメーション発生
		MainAttackState_FinalKeep,              //Final攻撃発生
		MainAttackState_FinalLock,              //Final攻撃後硬直
	};

	//サブ側の攻撃ステート
	enum eSubAttackState
	{
		SubAttackState_None,

		SubAttackState_IndivBeforeOccurence,   //Indivアニメーション発生
		SubAttackState_IndivBeforeOccurenceAir,//Indiv空中アニメーション発生
		SubAttackState_IndivKeep,              //Indiv攻撃発生
		SubAttackState_IndivKeepAir,           //Indiv空中攻撃発生
		SubAttackState_IndivLock,              //Indiv攻撃後硬直
		SubAttackState_IndivLockAir,           //Indiv空中攻撃後硬直

		SubAttackState_FinalBeforeOccurence,   //Finalアニメーション発生
		SubAttackState_FinalKeep,              //Final攻撃発生
		SubAttackState_FinalLock,              //Final攻撃後硬直
	};


	eCommonState _commonState = eCommonState::Common_None;
	eMainState _mainState = eMainState::Main_Idle;
	eSubState _subState = eSubState::Sub_Idle;
	eMainAttackState _mainAttackState = eMainAttackState::MainAttackState_None;
	eSubAttackState _subAttackState = eSubAttackState::SubAttackState_None;

	eInput_State inputState = eInput_State::Input_Idle;
	eInput_State preInputState = eInput_State::Input_Idle;

	hitInfo _hitInfo;//ステージとの衝突情報

	//各マップチップに接触した時のリアクション
	void ReactToMapChip(hitInfo info, bool isHitSlope);
	void ReactionEnter(Entity& other)override;
	bool accelerateEvaluate();
	void ControlCamera();

	std::shared_ptr<RectangleCollider> _collider = nullptr;//自分の当たり判定
	std::shared_ptr<SubPlayer> _subPlayer = nullptr;//サブプレイヤー

	std::shared_ptr<CoopAttack> _coopAttack = nullptr;//協力攻撃

	std::shared_ptr<IndivAttack> _mainIndivAttack = nullptr;//メイン個人技
	std::shared_ptr<IndivAttack> _subIndivAttack;           //サブ個人技

	std::shared_ptr<FinalAttack> _mainFinalAttack = nullptr; //メイン必殺技
	std::shared_ptr<FinalAttack> _subFinalAttack = nullptr;  //サブ必殺技

	std::shared_ptr<SoundEmitter> _soundEmitter = nullptr;
	std::shared_ptr<FluidInteractAdvection> _mainFluidInteractAdvection = nullptr;
	std::shared_ptr<FluidInteractAdvection> _subFluidInteractAdvection = nullptr;
	std::shared_ptr<FluidInteractHeat> _fluidInteractHeat = nullptr;

	PlayerGuideUI playerUI;
public:

	friend class SubPlayer;//メインとサブは友達！

	Player(IdInfo idInfo);
	virtual ~Player();
	void Init()override;
	void Update()override;
	void Draw()const override;

	void AddMainEP(UINT ep);//メインのEPを加算する。引数は加算したいEP
	void AddSubEP(UINT ep);//サブのEPを加算する。引数は加算したいEP
	void AddHP(UINT hp);


	void ReceiveHitStopNotify(float duration, float magnitude);

	void ReciveInpulse();//衝撃を受ける

	//ダメージを受ける
	void Damage()
	{

	}
	//回復する
	void Heal()
	{

	}
	//死んでいる
	void Dead()
	{
		

	}
	//クリアした
	void StageClear()
	{

	}

	//eMainState getState() {  }
	XMFLOAT2 getJumpPos()const
	{
		return jumpPos;
	};
	bool getIsJump()const
	{
		return isJump;
	};

protected:
};
