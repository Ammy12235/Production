#include "EngineLoop.h"
#include "ILoopController.h"


#include "EntityManager.h"
#include "ComponentManager.h"
#include "Core/Process/Fps.h"
#include "Core/Camera/Camera.h"
#include "Core/Process/Process.h"
#include "Graphics/Texture/TextureFactory.h"
#include "XAudio2.h"

//モジュール
#include "Input/Input.h"
#include "Input/KeyBoard.h"
#include "Input/Mouse.h"
#include "Input/Pad.h"
#include "Audio/XAudio2.h"
#include "Audio/Sound.h"
#include "Audio/SoundFactory.h"
#include "Physics/Terrain/TerrainSystem.h"
#include "Physics/Collider/PhysicsSystem.h"
#include "Fluid/FluidSimulationSystem.h"
#include "Scene/ChangeEffectMover.h"	

#include "Scene/SceneFactory.h"
#include "FactoryManager.h"



bool EngineLoop::ExecuteEngineLoop() const
{
	DWRITE.Draw2DStart();//2D描画開始
	//==============================
	// 更新処理
	//==============================
	
	//--------------------------
	//　処理時間系の更新
	//--------------------------
	PROCESS.SetStartTime();//フレームレートの計測
	FPS.CalculationFps(); //FPSを計測する。FPS及びフレーム間の経過時間を測定
	FPS.CalculationFrameTime();

	//--------------------------
	//　入力処理系の更新
	//--------------------------
	Keyboard::GetInstance().update();//入力の更新
	Mouse::GetInstance().update();
	Pad::GetInstance().update();

	//--------------------------
	//　シーン関連の更新
	//--------------------------
	SceneFactory::GetInstance()._sceneStack.top()->update();//スタックの一番上を更新
	
	if (ILoopController::GetInstance().isCleanup)
	{
		CleanupAllSystem();
		ILoopController::GetInstance().CleanupEnd();
	}
	

	//エンジンの終了条件の判定
	if (ILoopController::GetInstance().isExit)return false;

	//ポーズ条件の判定
	if (ILoopController::GetInstance().isPause == false) {
		ChangeEffectMover::GetInstance().update();//シーンが遷移するときに動く

		//--------------------------
		//　エンティティの更新
		//--------------------------
		EntityManager::GetInstance().Update();//エンティティのUpdate
		EntityManager::GetInstance().PhysicsUpdate();//エンティティの速度、加速度、位置の更新（ここで位置が更新される）

		//--------------------------
		//　コンポーネントの更新を行うシステムの更新
		// ＊エンティティ内で削除等が行われるため、絶対にEntityManagerのUpdateの後に行うべき
		//--------------------------
		PhysicsSystem::GetInstance().Update();//衝突判定を更新
		PhysicsSystem::GetInstance().ExecuteCollision();//エンティティ同士の衝突判定の実行
		TerrainSystem::GetInstance().ExecuteCollision();//地形とエンティティの衝突判定の実行
		ComponentManager::GetInstance().Update();//コンポーネントのUpdate
		//GlobalSequencer::GetInstance().Update();グローバルシーケンサーの更新
		//サウンドの更新
		//ParticleSystem::GetInstance().Update();エフェクトの更新

		FluidSimulationSystem::GetInstance().Update();
		FluidSimulationSystem::GetInstance().ExecuteFluidSimulation();

		EntityManager::GetInstance().LateUpdate();//エンティティのLateUpdate

		//--------------------------
		//　削除処理
		//--------------------------
		EntityManager::GetInstance().Cleanup();//削除フラグの立ったエンティティの削除
		PhysicsSystem::GetInstance().Cleanup();//削除フラグの立ったコライダーの削除
		ComponentManager::GetInstance().Cleanup();//削除フラグの立ったコンポーネントの削除
		FluidSimulationSystem::GetInstance().Cleanup();

		CAMERA.update();//カメラの更新
	}
	ImGui_ImplDX11_NewFrame();// DearImGuiのフレームの開始
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	 

	//その他の更新
	//==============================
	// 描画処理(ドローコマンドの蓄積)(並列化するならここであるべき)
	//==============================

	D3D.setIsApplyPostEffect(true);
	D3D.setIsApplyLayer(true);
	D3D.Clear();//前回の描画結果をクリア

	
	TerrainSystem::GetInstance().RenderTerrain();//ステージの描画
	EntityManager::GetInstance().Render();//Entityの描画
	
	SceneFactory::GetInstance()._sceneStack.top()->draw();//スタックの一番上を描画
	ChangeEffectMover::GetInstance().draw();

	
	//==============================
	// GPUへドローコマンドを発行
	//==============================

	D3D.ExecuteDraw();//ドローコマンドを順次実行する

	//ポストプロセス処理
	//
	D3D.ApplyPostProcessing();//ポストプロセスを適用する
	
	
	D3D.ExecuteDrawCameraUI();//カメラ上に配置されたUIの描画コマンドを実行する


	//=============================
	// デバッグ関連の描画を実行 
	//=============================

	EntityManager::GetInstance().DrawEntityStatistics();
	EntityManager::GetInstance().DrawDebugEntity();
	PhysicsSystem::GetInstance().DrawDebugCollider();
	PhysicsSystem::GetInstance().DrawColliderStatistics();
	ComponentManager::GetInstance().DrawComponentStatistics();
	TEX_FAC.drawTexNum();//テクスチャーの総数表示
	
	PROCESS.SetEndTime();//CPU処理の計測終了地点
	PROCESS.DrawProcess();//かかった時間を描画
	FPS.DrawFps();//FPSの表示

	DWRITE.Draw2DEnd();//2D描画終了

	ImGui::Render();//DearImGuiの描画コマンドを生成
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	
	D3D.Present();//レンダリング結果を表示


	return true;

}

void EngineLoop::CleanupAllSystem()const
{
	EntityManager::GetInstance().CleanupAll();
	PhysicsSystem::GetInstance().CleanupAll();
	TerrainSystem::GetInstance().CleanupAll();
	ComponentManager::GetInstance().CleanupAll();
	FluidSimulationSystem::GetInstance().CleanupAll();
}

bool EngineLoop::InitializeEngine(HINSTANCE hInstance)
{
	//==============================
	// 汎用システムの初期化
	//==============================
	ILoopController::GetInstance();
	Fps::GetInstance();        //FPS初期化
	Process::GetInstance();    //処理時間計測初期化

	D3D_INIT di; di.hWnd = WINDOW::m_hWnd;
	if (FAILED(DIRECT3D11::GetInstance().Init(&di)))//描画機能初期化
	{
		MSG(L"Direct3D11の初期化失敗");
		return false;
	}

	Camera::GetInstance(); //カメラ初期化

	if (!Input::GetInstance().Initalize(WINDOW::m_hWnd, hInstance))//入力機能初期化
	{
		MSG(L"Inputの初期化失敗");
		return false;
	}

	
	XAudio2::GetInstance().Init();//サウンド機能の初期化

	// Setup Dear ImGui context

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();

	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(WINDOW::m_hWnd);
	ImGui_ImplDX11_Init(D3D.GetDevice(), D3D.GetDeviceContext());

	DWRITE.CreateFontHandle(L"BIZ UDゴシック Bold", 20);
	DWRITE.CreateFontHandle(L"MS 明朝", 20);
	DWRITE.CreateFontHandle(L"Cascadia Code", 50);
	DWRITE.CreateFontHandle(L"Cascadia Code", 100);
	DWRITE.CreateFontHandle(L"Cascadia Code", 200);

	//シーン管理システムの初期化
	ChangeEffectMover::GetInstance();
	SceneFactory::GetInstance();

	//リソース生成ファクトリを明示的に初期化（リソースがない状態で使用されないように）
	TextureFactory::GetInstance();
	SoundFactory::GetInstance();

	//==============================
	// EntityComponent管理システムの初期化
	//==============================
	EntityManager::GetInstance();
	ComponentManager::GetInstance();
	TerrainSystem::GetInstance();
	PhysicsSystem::GetInstance();
	FactoryManager::GetInstance();//アセットマネージャへ命名変更予定

	FluidSimulationSystem::GetInstance().Init(D3D.GetDevice(), D3D.GetDeviceContext(), 1024,1024);
	return true;
}

//エンジンの終了処理
void EngineLoop::FinalizeEngine()
{
	// Cleanup
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();


	//SOUND::DeleteInstance();
	SingletonFinalizer::finalize();//シングルトンとして生成された各機能クラスはここで一気に削除される。
	Log("終了処理は正常に行われました。\n");
}
