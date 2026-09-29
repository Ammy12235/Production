#include "DirectX.h"
#include "DEFINE.h"
#include "DIRECTOR.h"
#include "WINDOW.h"
#include "Camera.h"
#include "Texture.h"
#include "Fps.h"
#include "Process.h"
WCHAR str[512];
bool DIRECTOR::initialize(HINSTANCE hInstance)const
{

	//現在のディレクトリを取得
	wchar_t dir[1024];
	GetCurrentDirectory(sizeof(dir), dir);
	InitDirectory(dir);

	//ログの記録を始める
	OpenLog();
	//ウィンドウ初期化
	WINDOW win;
	if (FAILED(win.InitWindow(hInstance, 0, 0, Define::WIN_W, Define::WIN_H, (LPWSTR)Define::APP_NAME)))
		return false;

	//FPS初期化
	Fps::CreateInstance();
	if (!FPS.InitFps())return false;

	//描画機能初期化
	DIRECT3D11::CreateInstance();
	D3D_INIT di;
	di.hWnd = WINDOW::m_hWnd;
	if (FAILED(DIRECT3D11::GetInstance().Init(&di)))
	{
		MSG(L"Direct3Dの初期化失敗");
		return false;
	}

	//カメラ初期化
	Camera::CreateInstance();
	CAMERA.InitCamera();
	//各常駐タイプのファクトリークラスはここで初期化
	TextureFactory::CreateInstance();

	Process::CreateInstance();

	DWRITE.CreateFontHandle(L"メイリオ", 50);
	DWRITE.CreateFontHandle(L"MS 明朝", 20);

	SetDataDirectory();
	TEX_FAC.Load("enemy.png");
	TEX_FAC.Load("trump_heart.png");
	TEX_FAC.Load("trump_spade.png");
	TEX_FAC.Load("trump_clover.png");
	TEX_FAC.Load("trump_star.png");
	TEX_FAC.Load("trump_atlas.png");
	TEX_FAC.Load("trump_atlas_002.png");
	Log("初期化処理は正常に行われました\n");
	return true;
}
float timer = 0;
bool isInstancing = true;
const int mapSize = 1024;
int counter = 0;
void DIRECTOR::mainloop()const
{
	//メインメッセージループ
	MSG msg = { 0 };
	ZeroMemory(&msg, sizeof(msg));
	while (WM_QUIT != msg.message)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{

				break;
			}
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}

		}
		PROCESS.SetStartTime();
		//ゲーム処理の実行
		D3D.Clear();

		//FPSを計測する。FPS及びフレーム間の経過時間を測定
		FPS.CalculationFps();
		FPS.CalculationFrameTime();
		CAMERA.WriteCameraInfo2D();

		timer += 0.0016f;


		//前回の描画結果をクリア
		D3D.Clear();
		DWRITE.Draw2DStart();//2D描画開始
		CAMERA.SetCameraScl(1.015 + sin(timer), 1.015 + sin(timer));
		CAMERA.SetCameraPos(mapSize * 64 / 2, mapSize * 64 / 2);
		//FPSの表示
		FPS.DrawFps();

		D3D.SetAlignmentBlendDesc(25);

		//テクスチャを指定してインスタンシング描画を行う
		D3D.m_pInstancing->DrawInstancedBox(TEX_FAC.getTexture("trump_atlas.png"), mapSize, mapSize, 64);


		PROCESS.SetEndTime();
		PROCESS.DrawProcess();
		DWRITE.Draw2DEnd();//2D描画終了
		D3D.Present(1, 0);
	}

	finalize();
}

//デバイスなどの解放処理
void DIRECTOR::finalize()const
{
	Process::DeleteInstance();
	TextureFactory::DeleteInstance();
	DIRECT3D11::DeleteInstance();
	Camera::DeleteInstance();
	Fps::DeleteInstance();
	Log("終了処理は正常に行われました。\n");
}
