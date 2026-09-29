#include "DirectX.h"
#include "DEFINE.h"
#include "DIRECTOR.h"
#include "WINDOW.h"
#include "Camera.h"
#include "Texture.h"
#include "Fps.h"
#include <thread>

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

	DWRITE.CreateFontHandle(L"メイリオ", 30);
	DWRITE.CreateFontHandle(L"MS 明朝", 20);

	SetDataDirectory();
	/*
	*/
	TEX_FAC.Load("example.png");
	TEX_FAC.Load("OIP.jpg");
	TEX_FAC.Load("ShimmerData.png");
	TEX_FAC.Load("FrictionNoize.png");
	Log("初期化処理は正常に行われました\n");
	return true;
}

float c = 0;
float d = 0;
float e = 0;
float f = 0.0f;
float g = 0.0f;
float h = 0.0f;
bool isBlur = 0;
bool isAddDynamic = 0;
bool isAddStatic = 0;
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
		//ゲーム処理の実行
		// 
		//前回の描画結果をクリア
		D3D.Clear();
		DWRITE.Draw2DStart();//2D描画開始
		CAMERA.SetCameraRot(0);
		//FPSを計測する。FPS及びフレーム間の経過時間を測定
		FPS.CalculationFps();
		FPS.CalculationFrameTime();
		c += 0.01f;
		h += 0.0001f;
		d = (0.5f + cos(c / 3) / 2);
		e += 0.3f;
		g += 0.001f;
		CAMERA.WriteCameraInfo2D();
		LARGE_INTEGER freq;
		QueryPerformanceFrequency(&freq);

		LARGE_INTEGER start, end;

		QueryPerformanceCounter(&start);
		//=================================================
		// BaseShading
		//=================================================
		{

			// レンダーターゲットサーフェスを切り替える
			D3D.GetDeviceContext()->OMSetRenderTargets(1, &D3D.g_pRTV, nullptr);
			// レンダーターゲットビューをクリア
			float ClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
			D3D.GetDeviceContext()->ClearRenderTargetView(D3D.g_pRTV, ClearColor);
			// 深度バッファをクリア
			D3D.GetDeviceContext()->ClearDepthStencilView(D3D.g_pDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);
			//描画
			int i = (int)g % 3;
			switch (i)
			{
			case 0:
				D3D.DrawRotImage(TEX_FAC.getTexture("example.png"), 640, 480, 1280, 960, 0, 1);
				break;
			case 1:
				D3D.DrawRotImage(TEX_FAC.getTexture("OIP.jpg"), 640, 480, 1280, 960, 0, 1);
				break;
			case 2:
			{
				D3D.DrawRotImage(TEX_FAC.getTexture("FrictionNoize.png"), 640, 480, 1280, 960, 0, 1);
				DWRITE.DrawFormatText(L"摩擦のシミュレーションに使われているテクスチャです。\nこのノイズに沿って水滴が流れていきます。", 100, 100, 460, 30, D3D.GetColor(255, 255, 255), 1, 1);
			}
			break;
			}


			if (e > 1)
			{
				isAddDynamic = true;
				e = 0;
			}
			if (c > 0.01f)
			{
				isAddStatic = true;
				c = 0.0f;
			}
			D3D.m_pDropWaterFilter->SetConstantBuffers(0.04f, 0.02f,
				XMFLOAT2((float)(rand() % 100) * 0.01f, (float)(rand() % 100) * 0.01f),
				XMFLOAT2((float)(rand() % 120) * 0.01f, (float)(rand() % 120) * 0.01f), isAddDynamic, isAddStatic);

			D3D.m_pDropWaterFilter->Render(D3D.g_pRTV, D3D.g_pRenderTargetView);

			
		}

		D3D.GetDeviceContext()->OMSetRenderTargets(1, &D3D.g_pRenderTargetView, nullptr);
		// レンダーターゲットサーフェスを切り替える

		
		//FPSの表示
		FPS.DrawFps();
		QueryPerformanceCounter(&end);

		double time = static_cast<double>(end.QuadPart - start.QuadPart) * 1000.0 / freq.QuadPart;
		WCHAR timeStr[256];
		swprintf(timeStr, 256, L"Time %3.3lf[ms]\n", time);
		DWRITE.DrawFormatText(timeStr, Define::WIN_W - 160, Define::WIN_H - 100, 200, 30, D3D.GetColor(255, 255, 255), 1, 1);
		DWRITE.Draw2DEnd();//2D描画終了
		D3D.Present(1, 0);

		isAddDynamic = false;
		isAddStatic = false;
	}
	
	
	finalize();
	
}


//デバイスなどの解放処理
//入力、描画、音響、デバッグ、ネットなど
void DIRECTOR::finalize()const
{

	TextureFactory::DeleteInstance();
	DIRECT3D11::DeleteInstance();
	Camera::DeleteInstance();
	Fps::DeleteInstance();
	Log("終了処理は正常に行われました。\n");
}
