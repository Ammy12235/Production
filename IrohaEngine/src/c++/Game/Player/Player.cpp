#include "Player.h"
#include "Enemy/IEnemy.h"
#include "UI/EPPopUp.h"
#include "UI/HPPopUp.h"

Player::Player(IdInfo idInfo)
{

	_idInfo._tag = "MainPlayer";

	//画像読み込み
	_texId = TEX_FAC.CreateTexture("tex/player/hal_dot.png");
	_finalAttackMainTex = TEX_FAC.CreateTexture("tex/player/hal_raf.jpg");
	_finalAttackSubTex = TEX_FAC.CreateTexture("tex/player/mei_raf.jpg");


	//音声読み込み
	_jumpStartSE = SND_FAC.CreateSound("snd/se/jump_start_0.mp3");
	//_jumpEndSE            = SND_FAC.CreateSound("snd/se/jump_end.wav");
	_attackActivatedAirSE = SND_FAC.CreateSound("snd/se/attack_air_0.wav");
	_attackActivatedSE = SND_FAC.CreateSound("snd/se/attack_0.wav");
	_finalAttackActivatedSE = SND_FAC.CreateSound("snd/se/finalAttack_activated.wav");
	_flowActivatedSE = SND_FAC.CreateSound("snd/se/flow_activated.wav");
	//_afterJumpGroundSE    = SND_FAC.CreateSound("snd/se/jump_end.mp3");
	_downSE = SND_FAC.CreateSound("snd/se/down.wav");
	_gainEPSE = SND_FAC.CreateSound("snd/se/gain_energy.wav");
	_gainHPSE = SND_FAC.CreateSound("snd/se/gain_hp.wav");
	_penaltyEPSE = SND_FAC.CreateSound("snd/se/ep_penalty.wav");
	_damagedSE = SND_FAC.CreateSound("snd/se/damaged.wav");

	_collider = AddComponent<RectangleCollider>();
	_soundEmitter = AddComponent<SoundEmitter>();

	_subPlayer = Instantiate<SubPlayer>();
	_coopAttack = Instantiate<CoopAttack>();

	_mainIndivAttack = Instantiate<IndivAttack>();
	_subIndivAttack = Instantiate<IndivAttack>();

	_mainFinalAttack = Instantiate<FinalAttack>();
	_subFinalAttack = Instantiate<FinalAttack>();

	_mainFluidInteractAdvection = AddComponent<FluidInteractAdvection>();
	_subFluidInteractAdvection = AddComponent<FluidInteractAdvection>();
	_mainFluidInteractAdvection->SetVelocity(XMFLOAT2(0, 0));
	_subFluidInteractAdvection->SetVelocity(XMFLOAT2(0, 0));
	_subFluidInteractAdvection->SetOwner(_subPlayer.get());

	_fluidInteractHeat = AddComponent<FluidInteractHeat>();
	_fluidInteractHeat->SetHeatValue(0);

	CAMERA.SetCameraScl(0.6f);
}

Player::~Player()
{

}

void Player::Init()
{
	//位置などのパラメータの初期化
	_position.x = 1000;
	_position.y = -2400;
	CAMERA.SetCameraPos(XMFLOAT2((_position.x - Define::WIN_W / 2), (_position.x)));
	_subPlayer.get()->SetPosition(_position);
	_subPlayer.get()->SetMainPlayer(this);

	//攻撃判定に使用するエンティティの初期化

	//----------------------------------------
	// 同時攻撃判定
	//----------------------------------------
	_coopAttack->SetPosition(_position);
	_coopAttack->SetScale(coopAttackRange);
	_coopAttack->SetActive(false);
	_coopAttack->SetDamage(coopAttackValue);

	//メインとサブを同時攻撃判定に登録
	_coopAttack->SetAttackEntity(*this);
	_coopAttack->SetAttackEntity(*_subPlayer);

	//----------------------------------------
	// メイン個人攻撃判定
	//----------------------------------------

	_mainIndivAttack->SetPosition(_position);
	_mainIndivAttack->SetScale(mainIndivAttackRange);//範囲
	_mainIndivAttack->SetDamage(mainIndivAttackValue);//与えるダメージ量
	_mainIndivAttack->SetActive(false);

	_mainIndivAttack->SetAttackEntity(*this);//使用するプレイヤー

	//----------------------------------------
	// サブ個人攻撃判定
	//----------------------------------------
	_subIndivAttack->SetPosition(_position);
	_subIndivAttack->SetScale(subIndivAttackRange);
	_subIndivAttack->SetDamage(subIndivAttackValue);
	_subIndivAttack->SetActive(false);

	_subIndivAttack->SetAttackEntity(*_subPlayer);


	//----------------------------------------
	// メイン必殺技判定
	//----------------------------------------
	_mainFinalAttack->SetPosition(_position);
	_mainFinalAttack->SetScale(mainFinalAttackRange);
	_mainFinalAttack->SetDamage(mainFinalAttackValue);
	_mainFinalAttack->SetActive(false);

	_mainFinalAttack.get()->SetAttackEntity(*this);

	//----------------------------------------
	// サブ必殺技判定
	//----------------------------------------
	_subFinalAttack->SetPosition(_position);
	_subFinalAttack->SetScale(subFinalAttackRange);
	_subFinalAttack->SetDamage(subFinalAttackValue);
	_subFinalAttack->SetActive(false);

	_subFinalAttack->SetAttackEntity(*_subPlayer);

	_a.x = 0.4f;
	_v.x = 0;
	_v.y = 0;
	maxSpeed = defaultMaxSpeed;
	width = 64;
	height = 64;
	jumpPower = 0.0f;

	_commonState = Common_None;
	_mainState = Main_Idle;
	_subState = Sub_Idle;
	_mainAttackState = MainAttackState_None;
	_subAttackState = SubAttackState_None;

	maxHP = 100;
	currentHP = 100;

	mainMaxEP = 100;
	mainCurrentEP = 25;

	subMaxEP = 100;
	subCurrentEP = 25;

	cameraPos = _position;
	cameraTimer = 0.0f;

}

bool Player::accelerateEvaluate()//地面では動きやすく止まりやすく、空中では動きにくく止まりにくくする処理
{
	if (inputState != preInputState)
	{
		accelTimer = 0.0f;
	}

	if (isFloatingAir)_flyingRegist = 0.4f;
	else _flyingRegist = 1.0f;
	if (inputState == Input_Right)
	{
		_v.x += _a.x * _flyingRegist;
		if (_v.x > maxSpeed)_v.x = maxSpeed;
	}
	else if (inputState == Input_Left)
	{
		_v.x -= _a.x * _flyingRegist;
		if (_v.x < -maxSpeed)_v.x = -maxSpeed;
	}
	else if (inputState == Input_Idle)
	{
		if (_v.x > 0)
		{
			accelTimer += 0.016f; _v.x -= accelTimer * _flyingRegist;
			if (_v.x < 0.1f)_v.x = 0.0f;
		}
		else if (_v.x < 0)
		{
			accelTimer += 0.016f; _v.x += accelTimer * _flyingRegist;
			if (_v.x > -0.1f)_v.x = 0.0f;
		}
	}

	return true;
}

void Player::ControlCamera()//プレイヤーに追従するカメラの挙動。ジャンプ時はY方向のカメラ移動を抑制する。
{

	if (GameManager::GetInstance().GetOpeningState() == eOpeningState::OpeningState_GameGuide_Delay)
	{
		cameraPos = _position;
		cameraPos += cameraOpeningOffset;
		cameraOpeningOffset.y += 1;
		CAMERA.SetCameraPos(XMFLOAT2((cameraPos.x - Define::WIN_W / 2), (cameraPos.y - Define::WIN_H)));
	}
	else
	{

		XMFLOAT2 diff = { 0.0f ,0.0f };
		float damping = sqrt(0.1f);
		XMFLOAT2 idealPos = GetPosition();
		XMFLOAT2 idealPrePos = GetPrePosition();
		diff = idealPos - idealPrePos;
		idealPos.x += diff.x * 25;
		idealPos.y += diff.y * 5;
		XMFLOAT2 jumpPos = getJumpPos();
		bool isJump = getIsJump();
		if (isPreJump != isJump)
		{
			cameraTimer = 0.0f;
		}
		if (isJump && cameraTimer <= 0.8f)
		{
			cameraTimer += 0.016f;
			idealPos.y = (3 * idealPos.y + 7 * jumpPos.y) / 10;
		}

		diff = (idealPos - cameraPos) / 30;

		if (abs(diff.x) < 0.1f)diff.x = 0.0f;
		if (abs(diff.y) < 0.1f)diff.y = 0.0f;
		cameraVel = diff;
		cameraPos += cameraVel;

		CAMERA.SetCameraPos(XMFLOAT2((cameraPos.x - Define::WIN_W / 2), (cameraPos.y - Define::WIN_H)));

		isPreJump = isJump;
	}
}

void Player::Update()
{

	if (GameManager::GetInstance().GetState() == eScoreModeState::ScoreModeState_GameOver ||
		GameManager::GetInstance().GetState() == eScoreModeState::ScoreModeState_Result)
	{
		SetVelocity(XMFLOAT2(0, 0));
		isMainVisible = true;
		isSubVisible = true;
		return;
	}

	//地形との当たり判定
	bool isHitSlope = false;
	TerrainSystem::GetInstance().GetStage()->collideStage(*this, _hitInfo, &isFloatingAir, &isHitSlope, width, height);
	ReactToMapChip(_hitInfo, isHitSlope);

	//=====================================================================
	//カメラ制御
	//=====================================================================
	ControlCamera();
	//=====================================================================
	//フロー管理
	//=====================================================================
	//メイン
	if ((float)mainCurrentEP > (float)mainMaxEP / 2)
	{
		isMainFlowActivated = true;//EPが半分以上溜まるとメインフロー解禁
		if (isMainFlowActivatedOnce)
		{
			_soundEmitter->Play(_flowActivatedSE);
			isMainFlowActivatedOnce = false;
		}
	}
	else
	{
		isMainFlowActivatedOnce = true;
		isMainFlowActivated = false;
	}

	//サブ
	if ((float)subCurrentEP > (float)subMaxEP / 2)
	{
		isSubFlowActivated = true;//EPが半分以上溜まるとメインフロー解禁
		if (isSubFlowActivatedOnce)
		{
			_soundEmitter->Play(_flowActivatedSE);
			isSubFlowActivatedOnce = false;
		}
	}
	else
	{
		isSubFlowActivatedOnce = true;
		isSubFlowActivated = false;
	}

	//=====================================================================
	//必殺技フラグ管理
	//=====================================================================
	//メインは必殺技を出せる状態かどうか
	if (isMainIndivEnabled && isMainFlowActivated)//個人攻撃可能で、かつフロー状態で、
	{
		if (_mainAttackState != MainAttackState_FinalBeforeOccurence//すでに必殺技を打っていないことを確認
			&& _mainAttackState != MainAttackState_FinalKeep
			&& _mainAttackState != MainAttackState_FinalLock)
		{

			isMainFinalEnabled = true; //メインは必殺技が出せる
		}
	}
	else
	{
		isMainFinalEnabled = false;
	}

	//サブは必殺技を出せる状態かどうか
	if (isSubIndivEnabled && isSubFlowActivated)//個人攻撃可能で、かつフロー状態で、
	{
		if (_subAttackState != SubAttackState_FinalBeforeOccurence//すでに必殺技を打っていないことを確認
			&& _subAttackState != SubAttackState_FinalKeep
			&& _subAttackState != SubAttackState_FinalLock)
		{
			isSubFinalEnabled = true;//サブは必殺技が出せる
		}

	}
	else
	{
		isSubFinalEnabled = false;
	}

	//攻撃判定の位置をプレイヤーの位置に合わせる

	_coopAttack->SetPosition(_position);


	//メインの分はメインに。
	_mainIndivAttack->SetPosition(_position);
	_mainFinalAttack->SetPosition(_position);

	//サブの分はサブに合わせる
	_subIndivAttack->SetPosition(_subPlayer->GetPosition());
	_subFinalAttack->SetPosition(_subPlayer->GetPosition());

	//=====================================================================
	// ヒットストップ処理
	//=====================================================================
	if (isMainHitStop)
	{
		_v = { 0,0 };
		mainHitStopTimer += FPS.GetFrameSecondTime();
		//少し揺らす
		int hitStopFactor = 10;
		mainHitStopOffset.x = (float)(hitStopFactor - rand() % hitStopFactor * 2) / hitStopFactor * 2 * mainHitStopMagnitude;
		if (isFloatingAir)//空中ならば全方向に揺れる
		{
			mainHitStopOffset.y = (float)(hitStopFactor - rand() % hitStopFactor * 2) / hitStopFactor * 2 * mainHitStopMagnitude;
		}
		else
		{
			mainHitStopOffset.y = 0;
		}

		if (mainHitStopTimer > mainHitStopTime)//ヒットストップ時間が終了したら、
		{
			mainHitStopTimer = 0.0f;
			mainHitStopOffset = { 0,0 };
			_v = mainHitStopStoreVelocity;//保存しておいた速度に置き換えてヒットストップを終了する
			isMainHitStop = false;//攻撃の更新が再開される
		}
	}
	if (isSubHitStop)
	{
		_subPlayer->SetVelocity(XMFLOAT2(0, 0));
		subHitStopTimer += FPS.GetFrameSecondTime();
		//少し揺らす
		int hitStopFactor = 10;
		subHitStopOffset.x = (float)(hitStopFactor - rand() % hitStopFactor * 2) / hitStopFactor * 2 * subHitStopMagnitude;
		if (isFloatingAir)//空中ならば全方向に揺れる
		{
			subHitStopOffset.y = (float)(hitStopFactor - rand() % hitStopFactor * 2) / hitStopFactor * 2 * subHitStopMagnitude;
		}
		else
		{
			subHitStopOffset.y = 0;
		}
		if (subHitStopTimer > subHitStopTime)
		{
			subHitStopTimer = 0.0f;
			subHitStopOffset = { 0,0 };
			_subPlayer->SetVelocity(subHitStopStoreVelocity); // 保存しておいた速度に置き換えてヒットストップを終了する
			isSubHitStop = false;//攻撃の更新が再開される
		}
	}
	//=====================================================================
	// 無敵処理
	//=====================================================================
	//もし、スタンして無敵時間を得たならば、ここで時間計測する。

	if (_mainState == Main_Damaged)
	{
		mainInvincibleTimer -= FPS.GetFrameSecondTime();
		mainInvincibleFlushTimer += FPS.GetFrameSecondTime();
		if (mainInvincibleFlushTimer > invincibleFlushTime)
		{
			isMainVisible = isMainVisible == false ? true : false;
			mainInvincibleFlushTimer = 0.0f;
		}
		if (mainInvincibleTimer < stunInvincibleTime)
		{
			_mainState = Main_Invicble;//スタン状態を抜けて動ける

		}
	}

	if (_mainState == Main_Invicble)
	{
		mainInvincibleTimer -= FPS.GetFrameSecondTime();
		mainInvincibleFlushTimer += FPS.GetFrameSecondTime();
		if (mainInvincibleFlushTimer > invincibleFlushTime)
		{
			isMainVisible = isMainVisible == false ? true : false;
			mainInvincibleFlushTimer = 0.0f;
		}
		if (mainInvincibleTimer < 0)//無敵時間を使い切ったのならば
		{
			mainInvincibleTimer = 0.0f;
			_mainState = Main_Idle;
			isMainInvincible = false;
			isMainVisible = true;
		}
	}

	if (_subState == Sub_Damaged)
	{
		subInvincibleTimer -= FPS.GetFrameSecondTime();
		subInvincibleFlushTimer += FPS.GetFrameSecondTime();
		if (subInvincibleFlushTimer > invincibleFlushTime)
		{
			isSubVisible = isSubVisible == false ? true : false;
			subInvincibleFlushTimer = 0.0f;
		}
		if (subInvincibleTimer < stunInvincibleTime)
		{
			_subState = Sub_Invicble;//スタン状態を抜けて動ける

		}
	}

	if (_subState == Sub_Invicble)
	{
		subInvincibleTimer -= FPS.GetFrameSecondTime();
		subInvincibleFlushTimer += FPS.GetFrameSecondTime();
		if (subInvincibleFlushTimer > invincibleFlushTime)
		{
			isSubVisible = isSubVisible == false ? true : false;
			subInvincibleFlushTimer = 0.0f;
		}
		if (subInvincibleTimer < 0)//無敵時間を使い切ったのならば
		{
			subInvincibleTimer = 0.0f;
			_subState = Sub_Idle;
			isSubInvincible = false;
			isSubVisible = true;
		}
	}

	//=====================================================================
	//基本操作制御
	//=====================================================================
	if (isMainHitStop == false)
	{

		XMFLOAT2 joyStickInput = Pad::GetInstance().GetAnalogStickInput();
		//キー入力の操作
		if (Keyboard::GetInstance().GetKey(DIK_RIGHTARROW) || joyStickInput.x > 0 && abs(joyStickInput.y) < 21000)//右に入力
		{
			inputState = Input_Right;
		}
		else if (Keyboard::GetInstance().GetKey(DIK_LEFTARROW) || joyStickInput.x < 0 && abs(joyStickInput.y) < 21000)//左に入力
		{
			inputState = Input_Left;
		}
		else
		{
			inputState = Input_Idle;
		}

		accelerateEvaluate();
	}

	//=====================================================================
	// ジャンプ処理
	//=====================================================================

	if (isMainHitStop == false)
	{
		if ((Keyboard::GetInstance().GetKeyDown(DIK_SPACE) || Pad::GetInstance().GetPadDown(ePad::jump)) && (!isJump && !isFloatingAir))//ジャンプの入力
		{
			jumpPos = _position;
			isJump = true;
			isFloatingAir = true;
			jumpPower = defaultJumpPower + jumpPowerFactor;
			_v.y -= jumpPower;
			_fluidInteractHeat->SetOffset(XMFLOAT2(0, 32));
			_soundEmitter->Play(_jumpStartSE);
		}
	}
	//=====================================================================
	//メインのステート管理
	//=====================================================================

	//攻撃管理

	//協力攻撃が出せる状態かどうか
	if (_commonState != Common_Death &&//死亡状態でなく、
		(_mainState != Main_Damaged && _subState != Sub_Damaged) &&     //メインとサブがダメージを受けてスタンしていなく、

		(_mainAttackState != MainAttackState_IndivKeep && _mainAttackState != MainAttackState_IndivKeepAir
			&& _mainAttackState != MainAttackState_IndivLock) &&        //メインが個人攻撃を出していなく、(重要なのは発生前ならばキャンセルが利くということ)

		(_subAttackState != SubAttackState_IndivKeep && _subAttackState != SubAttackState_IndivKeepAir
			&& _subAttackState != SubAttackState_IndivLock) &&            //サブも個人攻撃を出していなく、

		(_mainAttackState != MainAttackState_CoopBeforeOccurence && _mainAttackState != MainAttackState_CoopBeforeOccurenceAir
			&& _mainAttackState != MainAttackState_CoopKeep && _mainAttackState != MainAttackState_CoopKeepAir
			&& _mainAttackState != MainAttackState_CoopLock) &&				//すでに協力攻撃を出していなく、

		(_mainAttackState != MainAttackState_FinalBeforeOccurence && _mainAttackState != MainAttackState_FinalKeep
			&& _mainAttackState != MainAttackState_FinalLock) &&            //メインが必殺技を出していなく、

		(_subAttackState != SubAttackState_FinalBeforeOccurence && _subAttackState != SubAttackState_FinalKeep
			&& _subAttackState != SubAttackState_FinalLock))          //サブが必殺技を出していなければ、、
	{
		isCoopEnabled = true;//協力攻撃が出せる
	}
	else
	{
		isCoopEnabled = false;
	}

	//メインは個人攻撃が出せる状態かどうか 
	if (_commonState != Common_Death&&//死亡状態でなく、
		_mainState != Main_Damaged &&                                            //メインがダメージを受けてスタンしていなく、

		(_mainAttackState != MainAttackState_IndivKeep && _mainAttackState != MainAttackState_IndivKeepAir
			&& _mainAttackState != MainAttackState_IndivLock) &&        //メインが個人攻撃を出していなく、

		(_mainAttackState != MainAttackState_CoopBeforeOccurence && _mainAttackState != MainAttackState_CoopBeforeOccurenceAir
			&& _mainAttackState != MainAttackState_CoopKeep && _mainAttackState != MainAttackState_CoopKeepAir
			&& _mainAttackState != MainAttackState_CoopLock) &&			//すでに協力攻撃を出していない状態であれば、
		(_mainAttackState != MainAttackState_FinalBeforeOccurence && _mainAttackState != MainAttackState_FinalKeep
			&& _mainAttackState != MainAttackState_FinalLock))            //メインが必殺技を出していなく、必殺技を出していなければ、、//メインが必殺技を出していなく、
	{
		isMainIndivEnabled = true;//メインは個人攻撃が出せる
	}
	else
	{
		isMainIndivEnabled = false;
	}

	float deltaTime = FPS.GetFrameSecondTime();
	if (isMainHitStop == false/* && _mainState == Main_Damaged*/)//メインがヒットストップ状態、またはスタン状態であれば入力を受け付けない
	{

		//=====================================================================
		// 協力攻撃
		//=====================================================================
		if ((Keyboard::GetInstance().GetKeyDown(DIK_C) || Pad::GetInstance().GetPadDown(ePad::coop)) && isCoopEnabled)//協力攻撃
		{

			attackMainTimer = 0.0f;//攻撃タイマーを初期化

			if (isFloatingAir)//メイン側が空中にいるなら空中技にする
			{

				_coopAttack->SetOffset(XMFLOAT2(0, 0));
				_mainAttackState = MainAttackState_CoopBeforeOccurenceAir;
			}
			else//そうでないなら地上技
			{
				//オフセット補正向いている方向に
				if (isFlip)
				{
					_coopAttack->SetOffset(XMFLOAT2(64, 0));
					_fluidInteractHeat->SetOffset(XMFLOAT2(64, 0));
				}
				else
				{
					_coopAttack->SetOffset(XMFLOAT2(-64, 0));
					_fluidInteractHeat->SetOffset(XMFLOAT2(-64, 0));
				}
				_mainAttackState = MainAttackState_CoopBeforeOccurence;
			}
		}
		//--------------------------------------------------------------------
		// 地上協力攻撃
		//--------------------------------------------------------------------
		if (_mainAttackState == MainAttackState_CoopBeforeOccurence)
		{

			
			_mainFluidInteractAdvection->SetVelocity(XMFLOAT2(10 * cos(attackMainTimer * 30 - Define::PI / 2), 10 * sin(attackMainTimer * 30 - Define::PI / 2)));
			_mainFluidInteractAdvection->SetOffset(XMFLOAT2(200 * cos(attackMainTimer * 30), 100 * sin(attackMainTimer * 30)));
			_v.x *= 0.8f;//攻撃モーション中は減速する
			attackMainTimer += deltaTime;

			if (attackMainTimer > attackBeforeOccurrenceTime)
			{
				_mainAttackState = MainAttackState_CoopKeep;
				attackMainTimer = 0.0f;
				_soundEmitter->Play(_attackActivatedSE);
				_coopAttack->SetActive(true);//攻撃判定発生
				isMainInvincible = true;//メインを無敵にする
				isSubInvincible = true;//サブも無敵
			}
		}
		else if (_mainAttackState == MainAttackState_CoopKeep)
		{

			_v.x *= 0.1f;//攻撃モーション中は減速する
			
			_mainFluidInteractAdvection->SetVelocity(XMFLOAT2(10 * cos(attackMainTimer * 30 - Define::PI / 2), 10 * sin(attackMainTimer * 30 - Define::PI / 2)));
			_mainFluidInteractAdvection->SetOffset(XMFLOAT2(200 * cos(attackMainTimer * 30), 100 * sin(attackMainTimer * 30)));
			attackMainTimer += deltaTime;
			if (attackMainTimer > attackKeepTime)
			{
				
				_mainAttackState = MainAttackState_CoopLock;
				attackMainTimer = 0.0f;
				_mainFluidInteractAdvection->SetVelocity(XMFLOAT2(0,0));
				_mainFluidInteractAdvection->SetOffset(XMFLOAT2(0,0));
				_coopAttack->SetActive(false);//攻撃判定消滅
				isMainInvincible = false;//メインの無敵解除
				isSubInvincible = false;//サブも解除
			}
		}
		else if (_mainAttackState == MainAttackState_CoopLock)
		{
			_v.x *= 0.1f;//攻撃モーション中は減速する
			attackMainTimer += deltaTime;
			if (attackMainTimer > attackAfterLockTime)
			{
				_mainAttackState = MainAttackState_None;
				attackMainTimer = 0.0f;
			}
		}
		//--------------------------------------------------------------------
		// 空中協力攻撃
		//--------------------------------------------------------------------
		if (_mainAttackState == MainAttackState_CoopBeforeOccurenceAir)
		{
			attackMainTimer += deltaTime;
			if (attackMainTimer > attackBeforeOccurrenceTimeAir)
			{
				_mainAttackState = MainAttackState_CoopKeepAir;
				attackMainTimer = 0.0f;
				_soundEmitter->Play(_attackActivatedAirSE);
				_coopAttack->SetActive(true);//攻撃判定発生
				isMainInvincible = true;//メインを無敵にする
				isSubInvincible = true;//サブも無敵
			}
		}
		else if (_mainAttackState == MainAttackState_CoopKeepAir)
		{
			_mainFluidInteractAdvection->SetVelocity(XMFLOAT2(10 * cos(attackMainTimer * 30 - Define::PI / 2), 10 * sin(attackMainTimer * 30 - Define::PI / 2)));
			_mainFluidInteractAdvection->SetOffset(XMFLOAT2(150 * cos(attackMainTimer * 30), 150 * sin(attackMainTimer * 30)));
			attackMainTimer += deltaTime;
			if (attackMainTimer > attackKeepTimeAir)
			{
				_mainAttackState = MainAttackState_CoopLockAir;
				attackMainTimer = 0.0f;
				_mainFluidInteractAdvection->SetVelocity(XMFLOAT2(0, 0));
				_mainFluidInteractAdvection->SetOffset(XMFLOAT2(0, 0));
				_coopAttack->SetActive(false);//攻撃判定消滅
				isMainInvincible = false;//メインの無敵解除
				isSubInvincible = false;//サブも解除
			}
		}
		else if (_mainAttackState == MainAttackState_CoopLockAir)
		{
			attackMainTimer += deltaTime;
			if (attackMainTimer > attackAfterLockTimeAir)
			{
				_mainAttackState = MainAttackState_None;
				attackMainTimer = 0.0f;
			}
		}

		//=====================================================================
		// 個人攻撃メイン
		//=====================================================================
		if ((Keyboard::GetInstance().GetKeyDown(DIK_X) || Pad::GetInstance().GetPadDown(ePad::indivMain)) && isMainIndivEnabled)//個人攻撃メイン
		{
			

			attackMainTimer = 0.0f;//攻撃タイマーを初期化

			if (isFloatingAir)//メイン側が空中にいるなら空中技にする
			{
				_mainIndivAttack->SetOffset(XMFLOAT2(0, 0));
				_mainAttackState = MainAttackState_IndivBeforeOccurenceAir;
			}
			else//そうでないなら地上技

			{
				//オフセット補正向いている方向に
				if (isFlip)
				{
					_mainIndivAttack->SetOffset(XMFLOAT2(64, 0));
					_fluidInteractHeat->SetOffset(XMFLOAT2(64, 0));
				}
				else
				{
					_mainIndivAttack->SetOffset(XMFLOAT2(-64, 0));
					_fluidInteractHeat->SetOffset(XMFLOAT2(-64, 0));
				}
				_mainAttackState = MainAttackState_IndivBeforeOccurence;

			}
		}
		//---------------------------------------------------------------------
		// 地上個人攻撃メイン
		//---------------------------------------------------------------------
		if (_mainAttackState == MainAttackState_IndivBeforeOccurence)
		{
			_v.x *= 0.8f;//攻撃モーション中は減速する
			attackMainTimer += deltaTime;
			if (attackMainTimer > attackBeforeOccurrenceTime)
			{
				_mainAttackState = MainAttackState_IndivKeep;
				attackMainTimer = 0.0f;
				_mainIndivAttack->SetActive(true);//攻撃判定発生
				_soundEmitter->Play(_attackActivatedSE);
				isMainInvincible = true;//メインを無敵にする
				if (isFlip)
				{

				}
			}
		}
		else if (_mainAttackState == MainAttackState_IndivKeep)
		{
			_v.x *= 0.1f;//攻撃モーション中は減速する
			_mainFluidInteractAdvection->SetVelocity(XMFLOAT2(5 * cos(attackMainTimer * 30 - Define::PI / 2), 5 * sin(attackMainTimer * 30 - Define::PI / 2)));
			_mainFluidInteractAdvection->SetOffset(XMFLOAT2(100 * cos(attackMainTimer * 30), 100 * sin(attackMainTimer * 30)));
			attackMainTimer += deltaTime;
			if (attackMainTimer > attackKeepTime)
			{
				_mainAttackState = MainAttackState_IndivLock;
				_mainFluidInteractAdvection->SetVelocity(XMFLOAT2(0, 0));
				_mainFluidInteractAdvection->SetOffset(XMFLOAT2(0, 0));
				attackMainTimer = 0.0f;
				_mainIndivAttack->SetActive(false);//攻撃判定消滅
				isMainInvincible = false;//メインの無敵解除
			}
		}
		else if (_mainAttackState == MainAttackState_IndivLock)
		{
			_v.x *= 0.1f;//攻撃モーション中は減速する
			attackMainTimer += deltaTime;
			if (attackMainTimer > attackAfterLockTime)
			{
				_mainAttackState = MainAttackState_None;
				attackMainTimer = 0.0f;
			}
		}
		//---------------------------------------------------------------------
		// 空中個人攻撃メイン
		//---------------------------------------------------------------------
		if (_mainAttackState == MainAttackState_IndivBeforeOccurenceAir)
		{

			attackMainTimer += deltaTime;
			if (attackMainTimer > attackBeforeOccurrenceTimeAir)
			{
				_mainAttackState = MainAttackState_IndivKeepAir;
				attackMainTimer = 0.0f;
				_mainIndivAttack->SetActive(true);//攻撃判定発生
				_soundEmitter->Play(_attackActivatedAirSE);
				isMainInvincible = true;//メインを無敵にする
			}
		}
		else if (_mainAttackState == MainAttackState_IndivKeepAir)
		{
			_mainFluidInteractAdvection->SetVelocity(XMFLOAT2(2 * cos(attackMainTimer * 30 - Define::PI / 2), 2 * sin(attackMainTimer * 30 - Define::PI / 2)));
			_mainFluidInteractAdvection->SetOffset(XMFLOAT2(100 * cos(attackMainTimer * 30), 100 * sin(attackMainTimer * 30)));
			attackMainTimer += deltaTime;
			if (attackMainTimer > attackKeepTimeAir)
			{
				_mainFluidInteractAdvection->SetVelocity(XMFLOAT2(0, 0));
				_mainFluidInteractAdvection->SetOffset(XMFLOAT2(0, 0));
				_mainAttackState = MainAttackState_IndivLockAir;
				attackMainTimer = 0.0f;
				_mainIndivAttack->SetActive(false);//攻撃判定消滅
				isMainInvincible = false;//メインの無敵解除
			}
		}
		else if (_mainAttackState == MainAttackState_IndivLockAir)
		{
			attackMainTimer += deltaTime;
			if (attackMainTimer > attackAfterLockTimeAir)
			{
				_mainAttackState = MainAttackState_None;
				attackMainTimer = 0.0f;
			}
		}

		//=====================================================================
		// 必殺技メイン
		//=====================================================================
		Pad::GetInstance().SetPadVibration(0, 0);
		if ((Keyboard::GetInstance().GetKeyDown(DIK_S)||Pad::GetInstance().GetPadDown(ePad::finalMain)) && isMainFinalEnabled)
		{
			_mainFluidInteractAdvection->SetVelocity(XMFLOAT2(20, 20));
			attackMainTimer = 0.0f;//攻撃タイマーを初期化
			if ((int)mainCurrentEP - (int)finalAttackEPConsumeValue < 0)//０を超えて消費してしまう場合、
			{
				mainCurrentEP = 0;//メインのEPを０にする
			}
			else
			{
				mainCurrentEP -= finalAttackEPConsumeValue;//EPを消費
			}
			_mainAttackState = MainAttackState_FinalBeforeOccurence;
			isMainInvincible = true;//メインを無敵にする
			isSubInvincible = true;
		}
		if (_mainAttackState == MainAttackState_FinalBeforeOccurence)
		{
			_v.x *= 0.8f;//攻撃モーション中は減速する
			attackMainTimer += deltaTime;
			if (attackMainTimer > finalAttackBeforeOccurrenceTime)
			{
				_mainAttackState = MainAttackState_FinalKeep;
				attackMainTimer = 0.0f;
				_mainFinalAttack->SetActive(true);//攻撃判定発生
				_soundEmitter->Play(_finalAttackActivatedSE);
			}
		}
		else if (_mainAttackState == MainAttackState_FinalKeep)
		{
			_mainFluidInteractAdvection->SetVelocity(XMFLOAT2(attackMainTimer * 300 * cos(attackMainTimer * 30 - Define::PI / 2), attackMainTimer * 300 * sin(attackMainTimer * 30 - Define::PI / 2)));
			_mainFluidInteractAdvection->SetOffset(XMFLOAT2(attackMainTimer * 1000 * cos(attackMainTimer * 30), attackMainTimer * 1000 * sin(attackMainTimer * 30)));
			_v.x *= 0.1f;//攻撃モーション中は減速する
			attackMainTimer += deltaTime;
			if (attackMainTimer > finalAttackKeepTime)
			{
				_mainFluidInteractAdvection->SetVelocity(XMFLOAT2(0, 0));
				_mainFluidInteractAdvection->SetOffset(XMFLOAT2(0, 0));

				_mainAttackState = MainAttackState_FinalLock;
				attackMainTimer = 0.0f;
				_mainFinalAttack->SetActive(false);//攻撃判定消滅
			}
		}
		else if (_mainAttackState == MainAttackState_FinalLock)
		{
			_v.x *= 0.1f;//攻撃モーション中は減速する
			attackMainTimer += deltaTime;
			if (attackMainTimer > finalAttackAfterLockFlame)
			{
				_mainAttackState = MainAttackState_None;
				isMainInvincible = false;//メインの無敵解除
				isSubInvincible = false;
				attackMainTimer = 0.0f;
			}
		}
	}

	//=====================================================================
	//サブのステート管理
	//=====================================================================

	//サブは個人攻撃が出せる状態かどうか
	if (_commonState != Common_Death&&//死亡状態でなく、ダウン状態でもなく、
		_subState != Sub_Damaged &&                                            //サブがダメージを受けてスタンしていなく、

		(_subAttackState != SubAttackState_IndivKeep && _subAttackState != SubAttackState_IndivKeepAir
			&& _subAttackState != SubAttackState_IndivLock) &&        //サブが個人攻撃を出していなく、

		(_mainAttackState != MainAttackState_CoopBeforeOccurence && _mainAttackState != MainAttackState_CoopBeforeOccurenceAir
			&& _mainAttackState != MainAttackState_CoopKeep && _mainAttackState != MainAttackState_CoopKeepAir
			&& _mainAttackState != MainAttackState_CoopLock) &&				//すでに協力攻撃を出していない状態であれば、
		(_subAttackState != SubAttackState_FinalBeforeOccurence && _subAttackState != SubAttackState_FinalKeep
			&& _subAttackState != SubAttackState_FinalLock))          //サブが必殺技を出していなければ、、
	{
		isSubIndivEnabled = true;//サブは個人攻撃が出せる
	}
	else
	{
		isSubIndivEnabled = false;
	}

	if (isSubHitStop == false/* && _subState == Sub_Damaged*/)//サブがヒットストップ状態、またはスタン状態であれば入力を受け付けない
	{
		//=====================================================================
		// 個人攻撃サブ
		//=====================================================================
		if ((Keyboard::GetInstance().GetKeyDown(DIK_Z) || Pad::GetInstance().GetPadDown(ePad::indivSub)) && isSubIndivEnabled)//個人攻撃サブ
		{

			attackSubTimer = 0.0f;//攻撃タイマーを初期化

			if (isFloatingAir)//サブ側が空中にいるなら空中技にする
			{
				_subIndivAttack->SetOffset(XMFLOAT2(0, 0));
				_subAttackState = SubAttackState_IndivBeforeOccurenceAir;
			}
			else//そうでないなら地上技
			{
				//オフセット補正向いている方向に
				if (isFlip)
				{
					_subIndivAttack->SetOffset(XMFLOAT2(64, 0));
					_fluidInteractHeat->SetOffset(XMFLOAT2(64, 0));
				}
				else
				{
					_subIndivAttack->SetOffset(XMFLOAT2(-64, 0));
					_fluidInteractHeat->SetOffset(XMFLOAT2(-64, 0));
				}
				_subAttackState = SubAttackState_IndivBeforeOccurence;
			}
		}
		//---------------------------------------------------------------------
		// 地上個人攻撃サブ
		//---------------------------------------------------------------------
		if (_subAttackState == SubAttackState_IndivBeforeOccurence)
		{
			XMFLOAT2 subVelocity = _subPlayer->GetVelocity();
			subVelocity *= 0.8f;//攻撃モーション中は減速する
			_subPlayer->SetVelocity(subVelocity);

			attackSubTimer += deltaTime;
			if (attackSubTimer > attackBeforeOccurrenceTime)
			{
				_subAttackState = SubAttackState_IndivKeep;
				attackSubTimer = 0.0f;
				_subIndivAttack.get()->SetActive(true);//攻撃判定発生
				_soundEmitter->Play(_attackActivatedSE);
				isSubInvincible = true;//サブを無敵にする
			}
		}
		else if (_subAttackState == SubAttackState_IndivKeep)
		{
			XMFLOAT2 subVelocity = _subPlayer->GetVelocity();
			subVelocity *= 0.1f;//攻撃モーション中は減速する
			_subPlayer->SetVelocity(subVelocity);
			_subFluidInteractAdvection->SetVelocity(XMFLOAT2(5 * cos(attackSubTimer * 30 - Define::PI / 2), 5 * sin(attackSubTimer * 30 - Define::PI / 2)));
			_subFluidInteractAdvection->SetOffset(XMFLOAT2(100 * cos(attackSubTimer * 30), 100 * sin(attackSubTimer * 30)));
			attackSubTimer += deltaTime;
			if (attackSubTimer > attackKeepTime)
			{
				_subAttackState = SubAttackState_IndivLock;
				_subFluidInteractAdvection->SetVelocity(XMFLOAT2(0, 0));
				_subFluidInteractAdvection->SetOffset(XMFLOAT2(0, 0));
				attackSubTimer = 0.0f;
				_subIndivAttack.get()->SetActive(false);//攻撃判定消滅
				isSubInvincible = false;//サブの無敵解除
			}
		}
		else if (_subAttackState == SubAttackState_IndivLock)
		{
			XMFLOAT2 subVelocity = _subPlayer->GetVelocity();
			subVelocity *= 0.1f;//攻撃モーション中は減速する
			_subPlayer->SetVelocity(subVelocity);
			attackSubTimer += deltaTime;
			if (attackSubTimer > attackAfterLockTime)
			{
				_subAttackState = SubAttackState_None;
				attackSubTimer = 0.0f;
			}
		}
		//---------------------------------------------------------------------
		// 空中個人攻撃サブ
		//---------------------------------------------------------------------
		if (_subAttackState == SubAttackState_IndivBeforeOccurenceAir)
		{

			attackSubTimer += deltaTime;
			if (attackSubTimer > attackBeforeOccurrenceTimeAir)
			{
				_subAttackState = SubAttackState_IndivKeepAir;
				attackSubTimer = 0.0f;
				_subIndivAttack.get()->SetActive(true);//攻撃判定発生
				_soundEmitter->Play(_attackActivatedAirSE);
				isSubInvincible = true;//サブを無敵にする
			}
		}
		else if (_subAttackState == SubAttackState_IndivKeepAir)
		{
			_subFluidInteractAdvection->SetVelocity(XMFLOAT2(2 * cos(attackSubTimer * 30 - Define::PI / 2), 2 * sin(attackSubTimer * 30 - Define::PI / 2)));
			_subFluidInteractAdvection->SetOffset(XMFLOAT2(100 * cos(attackSubTimer * 30), 100 * sin(attackSubTimer * 30)));
			attackSubTimer += deltaTime;
			if (attackSubTimer > attackKeepTimeAir)
			{
				_subFluidInteractAdvection->SetVelocity(XMFLOAT2(0, 0));
				_subFluidInteractAdvection->SetOffset(XMFLOAT2(0, 0));
				_subAttackState = SubAttackState_IndivLockAir;
				attackSubTimer = 0.0f;
				_subIndivAttack.get()->SetActive(false);//攻撃判定消滅
				isSubInvincible = false;//サブの無敵解除
			}
		}
		else if (_subAttackState == SubAttackState_IndivLockAir)
		{
			attackSubTimer += deltaTime;
			if (attackSubTimer > attackAfterLockTimeAir)
			{
				_subAttackState = SubAttackState_None;
				attackSubTimer = 0.0f;
			}
		}

		//=====================================================================
		// 必殺技サブ
		//=====================================================================
		if ((Keyboard::GetInstance().GetKeyDown(DIK_A)||Pad::GetInstance().GetPadDown(ePad::finalSub)) && isSubFinalEnabled)
		{
			_subFluidInteractAdvection->SetVelocity(XMFLOAT2(20, 20));
			attackSubTimer = 0.0f;//攻撃タイマーを初期化
			if ((int)subCurrentEP - (int)finalAttackEPConsumeValue < 0)//０を超えて消費してしまう場合、
			{
				subCurrentEP = 0;//サブのEPを０にする
			}
			else
			{
				subCurrentEP -= finalAttackEPConsumeValue;//EPを消費
			}
			_subAttackState = SubAttackState_FinalBeforeOccurence;
			isSubInvincible = true;//サブを無敵にする
			isMainInvincible = true;
		}
		if (_subAttackState == SubAttackState_FinalBeforeOccurence)
		{
			_v.x *= 0.8f;//攻撃モーション中は減速する
			attackSubTimer += deltaTime;
			if (attackSubTimer > finalAttackBeforeOccurrenceTime)
			{
				_subAttackState = SubAttackState_FinalKeep;
				attackSubTimer = 0.0f;
				_subFinalAttack.get()->SetActive(true);//攻撃判定発生
				_soundEmitter->Play(_finalAttackActivatedSE);

			}
		}
		else if (_subAttackState == SubAttackState_FinalKeep)
		{
			_subFluidInteractAdvection->SetVelocity(XMFLOAT2(attackSubTimer * 300 * cos(attackSubTimer * 30 - Define::PI / 2), attackSubTimer * 300 * sin(attackSubTimer * 30 - Define::PI / 2)));
			_subFluidInteractAdvection->SetOffset(XMFLOAT2(attackSubTimer * 1000 * cos(attackSubTimer * 30), attackSubTimer * 1000 * sin(attackSubTimer * 30)));
			_v.x *= 0.1f;//攻撃モーション中は減速する
			attackSubTimer += deltaTime;
			if (attackSubTimer > finalAttackKeepTime)
			{
				_subFluidInteractAdvection->SetVelocity(XMFLOAT2(0, 0));
				_subFluidInteractAdvection->SetOffset(XMFLOAT2(0, 0));
				_subAttackState = SubAttackState_FinalLock;
				attackSubTimer = 0.0f;
				_subFinalAttack.get()->SetActive(false);//攻撃判定消滅
			}
		}
		else if (_subAttackState == SubAttackState_FinalLock)
		{
			_v.x *= 0.1f;//攻撃モーション中は減速する
			attackSubTimer += deltaTime;
			if (attackSubTimer > finalAttackAfterLockFlame)
			{
				_subAttackState = SubAttackState_None;
				isSubInvincible = false;//サブの無敵解除
				isMainInvincible = false;
				attackSubTimer = 0.0f;
			}
		}
	}


	if (!isFloatingAir)
	{

		if (inputState == Input_Right)isFlip = true;
		else if (inputState == Input_Left) isFlip = false;
	}


	preInputState = inputState;//１フレーム分入力を保存

	_v.y += Define::Gravity;//重力の加算
	SetVelocity(_v);

}

// マップチップと接触したときのリアクション
void Player::ReactToMapChip(hitInfo info, bool isHitSlope)
{
	if (isHitSlope)
	{
		jumpPower = 0;
		isJump = false;
		_v.y = 0;
	}

	if (info.isHitLeft)
	{

	}
	if (info.isHitRight)
	{

	}
	if (info.isHitTop)
	{
		if (isFirstLand == false)//一番初めの着地をマネージャーに知らせる
		{
			GameManager::GetInstance().SetOpeningState(eOpeningState::OpeningState_GameGuide_Land);
			isFirstLand = true;
		}
		jumpPower = 0.0f;
		isJump = false;
		_v.y = 0;

	}
	if (info.isHitBottom)
	{

		jumpPower = 0.0f;
		_v.y = -1;
	}


}


void Player::Draw()const
{
	//プレイヤーの描画
	D3D.SetAlignmentBlendDesc(255);
	if (isMainVisible)
	{
		D3D.DrawRotFlipImage(_texId, _position.x + mainHitStopOffset.x, _position.y + mainHitStopOffset.y - 32, width, height, 0, 2, isFlip);
	}
	D3D.SetDefaultBlendDesc(255);
	if (Define::Debug)
	{
		int BaseY = 450;
		WCHAR s[512];
		swprintf(s, 256, L"PX=%f,PY=%f", _position.x, _position.y);
		DWRITE.DrawFormatText(s, 0, BaseY, 460, 30, D3D.GetColor(255, 255, 255), 1, 1);
		XMFLOAT2 vel = GetVelocity();
		swprintf(s, 256, L"PVX=%f,PVY=%f", vel.x, vel.y);
		DWRITE.DrawFormatText(s, 0, BaseY + 30, 460, 30, D3D.GetColor(255, 255, 255), 1, 1);
		swprintf(s, 256, L"isFloating=%d", isFloatingAir);
		DWRITE.DrawFormatText(s, 0, BaseY + 60, 460, 30, D3D.GetColor(255, 255, 255), 1, 1);
		swprintf(s, 256, L"isJump=%d", isJump);
		DWRITE.DrawFormatText(s, 0, BaseY + 90, 460, 30, D3D.GetColor(255, 255, 255), 1, 1);

		swprintf(s, 256, L"isCoopEnabled=%d", isCoopEnabled);
		DWRITE.DrawFormatText(s, 0, BaseY + 120, 460, 30, D3D.GetColor(255, 255, 255), 1, 1);

		swprintf(s, 256, L"isMainIndivEnabled=%d", isMainIndivEnabled);
		DWRITE.DrawFormatText(s, 0, BaseY + 150, 460, 30, D3D.GetColor(255, 255, 255), 1, 1);
		swprintf(s, 256, L"isSubIndivEnabled=%d", isSubIndivEnabled);
		DWRITE.DrawFormatText(s, 0, BaseY + 180, 460, 30, D3D.GetColor(255, 255, 255), 1, 1);

		swprintf(s, 256, L"isMainFinalEnabled=%d", isMainFinalEnabled);
		DWRITE.DrawFormatText(s, 300, BaseY + 150, 460, 30, D3D.GetColor(255, 255, 255), 1, 1);
		swprintf(s, 256, L"isSubFinalEnabled=%d", isSubFinalEnabled);
		DWRITE.DrawFormatText(s, 300, BaseY + 180, 460, 30, D3D.GetColor(255, 255, 255), 1, 1);

		swprintf(s, 256, L"isMainInvincible=%d", isMainInvincible);
		DWRITE.DrawFormatText(s, 0, BaseY + 210, 460, 30, D3D.GetColor(255, 255, 255), 1, 1);
		swprintf(s, 256, L"isSubInvincible=%d", isSubInvincible);
		DWRITE.DrawFormatText(s, 0, BaseY + 240, 460, 30, D3D.GetColor(255, 255, 255), 1, 1);

		swprintf(s, 256, L"CommonState=%d", _commonState);
		DWRITE.DrawFormatText(s, 0, BaseY + 270, 460, 30, D3D.GetColor(255, 255, 255), 1, 1);
		swprintf(s, 256, L"MainState=%d", _mainState);
		DWRITE.DrawFormatText(s, 0, BaseY + 300, 460, 30, D3D.GetColor(255, 255, 255), 1, 1);
		swprintf(s, 256, L"SubState=%d", _subState);
		DWRITE.DrawFormatText(s, 0, BaseY + 330, 460, 30, D3D.GetColor(255, 255, 255), 1, 1);

		swprintf(s, 256, L"HP=%d", currentHP);
		DWRITE.DrawFormatText(s, 0, BaseY + 360, 460, 30, D3D.GetColor(255, 255, 255), 1, 1);
		swprintf(s, 256, L"MainEP=%d", mainCurrentEP);
		DWRITE.DrawFormatText(s, 0, BaseY + 390, 460, 30, D3D.GetColor(255, 255, 255), 1, 1);
		swprintf(s, 256, L"SubEP=%d", subCurrentEP);
		DWRITE.DrawFormatText(s, 0, BaseY + 420, 460, 30, D3D.GetColor(255, 255, 255), 1, 1);

		swprintf(s, 256, L"CX=%f,CY=%f", cameraPos.x, cameraPos.y);
		DWRITE.DrawFormatText(s, 0, BaseY + 450, 460, 30, D3D.GetColor(255, 255, 255), 1, 1);
		swprintf(s, 256, L"CVX=%f,CVY=%f", cameraVel.x, cameraVel.y);
		DWRITE.DrawFormatText(s, 0, BaseY + 480, 460, 30, D3D.GetColor(255, 255, 255), 1, 1);
	}

	eScoreModeState state = GameManager::GetInstance().GetState();
	if (state != eScoreModeState::ScoreModeState_GameOver && state != eScoreModeState::ScoreModeState_Opening
		&& state != eScoreModeState::ScoreModeState_WeatherChanging && state != eScoreModeState::ScoreModeState_WaveStart
		&& state != eScoreModeState::ScoreModeState_WaveClear)
	{
		playerUI.DrawPlayerGuide(currentHP, maxHP, mainCurrentEP, mainMaxEP, subCurrentEP, subMaxEP,
			isMainFlowActivated, isSubFlowActivated, isCoopEnabled, isMainIndivEnabled, isSubIndivEnabled);
	}

}

//ほかのエンティティと衝突したときのリアクション
void Player::ReactionEnter(Entity& other) {
	XMFLOAT2 otherVelocity = other.GetVelocity();
	XMFLOAT2 otherPosition = other.GetPosition();
	XMFLOAT2 otherPrePosition = other.GetPrePosition();

	IdInfo info = other.GetIdInfo();

	if (info._tag == "Enemy")//敵の攻撃に接触したら、
	{

		if (isMainInvincible)return;//無敵フラグが立っているならば、攻撃を受けない

		//ダメージを受けられる状態なら
		if (_commonState == Common_None)
		{
			IEnemy* enemy = dynamic_cast<IEnemy*>(&other);
			EnemyParameter* param = enemy->GetEnemyParameter();//接触した敵のパラメータをみる
			float damage = param->touchAttack;
			//EPとHPを減らす。

			if ((float)currentHP - damage < 0)//もし、HPが０になる攻撃を受けたら、
			{
				currentHP = 0;//HPを０にする
				_commonState = Common_Death;//HPが0になったので死んでしまった。
				isMainInvincible = true;
				isSubInvincible = true;
				CameraShaker::GetInstance().GenerateShake(Shake_Random, 1, 30);//演出としてカメラを揺らす
				GameManager::GetInstance().SetState(eScoreModeState::ScoreModeState_GameOver);
				return;
			}


			_soundEmitter->Play(_damagedSE);

			mainInvincibleTimer = stunUncontrollableTime + stunInvincibleTime;
			//_mainState = Main_Damaged;
			currentHP -= damage;//HPは０で止まるかを考慮しない
			isMainInvincible = true;//メインの無敵解除
			_mainState = Main_Damaged;

		}
		else if (info._tag == "Item")
		{

		}

	}
}

void Player::ReceiveHitStopNotify(float duration, float magnitude)//ヒットストップを外部から通知されたことを受け取る関数
{
	isMainHitStop = true;            //ヒットストップフラグをオンにし
	mainHitStopTime = duration;      //ヒットストップ時間を設定し、
	mainHitStopMagnitude = magnitude;//ヒットストップの大きさを設定し、
	mainHitStopOffset = { 0 ,0 };     //ヒットストップのオフセットを初期化し、
	mainHitStopStoreVelocity = _v;   //ヒットストップ後の速度を保存しておく
}

void  Player::AddMainEP(UINT ep)//メインのEPを加算する。引数は加算したいEP
{
	//メインのEP加算
	mainCurrentEP += ep;
	auto epPopUp = Instantiate<EPPopUp>(_position, ep, true);
	epPopUp->SetOffset(XMFLOAT2(0, -64));

	_soundEmitter->Play(_gainEPSE);
	if (mainCurrentEP > mainMaxEP)mainCurrentEP = mainMaxEP;

}

void  Player::AddSubEP(UINT ep)//サブのEPを加算する。引数は加算したいEP
{
	//サブのEP加算
	subCurrentEP += ep;
	auto epPopUp = Instantiate<EPPopUp>(_position, ep, true);
	epPopUp->SetOffset(XMFLOAT2(0, -64));

	_soundEmitter->Play(_gainEPSE);

	if (subCurrentEP > subMaxEP)subCurrentEP = subMaxEP;
}

void Player::AddHP(UINT hp)
{

	currentHP += hp;
	auto epPopUp = Instantiate<EPPopUp>(_position, hp, true);
	epPopUp->SetOffset(XMFLOAT2(0, -64));

	_soundEmitter->Play(_gainHPSE);

	if (currentHP > maxHP)currentHP = maxHP;
}
