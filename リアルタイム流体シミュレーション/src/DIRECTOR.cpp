#include "DirectX.h"
#include "DEFINE.h"
#include "DIRECTOR.h"
#include "WINDOW.h"
#include "Camera.h"
#include "Texture.h"
#include "Fps.h"
#include "FluidSimulation.h"
#include "KeyBoard.h"
#include "Mouse.h"

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

	//入力機能初期化
	DirectInput::CreateInstance();
	if (!DINPUT.Initialize(WINDOW::m_hWnd, hInstance))
	{
		MSG(L"DirectInputの初期化失敗");
		return false;
	}

	DWRITE.CreateFontHandle(L"メイリオ", 30);
	DWRITE.CreateFontHandle(L"MS 明朝", 20);

	SetDataDirectory();
	TEX_FAC.Load("bomb.png");
	TEX_FAC.Load("InitPressure.png");
	TEX_FAC.Load("InitPressure_Ref.bmp");
	TEX_FAC.Load("InitPressure_Ref01.bmp");
	TEX_FAC.Load("uv-test.png");

	Log("初期化処理は正常に行われました\n");
	SetDataDirectory();
	std::ifstream ifs;
	std::array<std::vector<int>, 2> data;

	for (int i = 0; i < 2; i++)
	{
		ifs.open("map.csv");
		if (!ifs.is_open())
		{
			MSG(L"外部ファイルが読み込めません。:StageFactory");
		}
		std::string line;

		int xCount = 0;
		int yCount = 0;
		while (ifs.good()) {
			std::getline(ifs, line);

			// csvの分解処理（split）
			std::stringstream ss(line);
			std::string buff;
			while (std::getline(ss, buff, ','))
			{
				int mapId = std::atoi(buff.c_str());
				data[i].push_back(mapId);
				xCount++;
			}
			yCount++;
			xCount = 0;
		}
		ifs.close();
	}
	return true;
}
int texSize =1024;
void DIRECTOR::mainloop()const
{
	//メインメッセージループ
	MSG msg = { 0 };
	ZeroMemory(&msg, sizeof(msg));

	//シミュレーションモジュール生成
	FluidSimulation* g_fieldSimulation = new FluidSimulation();
	g_fieldSimulation->init(D3D.GetDevice(), D3D.GetDeviceContext(), texSize, texSize);
	//g_fieldSimulation->setInitVector(TEX_FAC.getTexture("InitPressure_Ref.bmp")->m_srv.Get(), TEX_FAC.getTexture("InitPressure_Ref01.bmp")->m_srv.Get());
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
		Keyboard::GetInstance()->update();
		Mouse::GetInstance()->update();
		//ゲーム処理の実行
		D3D.Clear();

		//FPSを計測する。FPS及びフレーム間の経過時間を測定
		FPS.CalculationFps();
		FPS.CalculationFrameTime();

		//シミュレーションに使う定数データをGPUに送ります。
		g_fieldSimulation->setConstantBuffer();

		//シミュレーションがスタートします
		g_fieldSimulation->execute();//実行します
		//前回の描画結果をクリア
		D3D.Clear();
		DWRITE.Draw2DStart();//2D描画開始
		//FPSの表示
		FPS.DrawFps();
		FPS.DrawFrameTime();
		POINT p;
		Mouse::GetInstance()->GetMouseCursorPos(&p);
		XMFLOAT2 mouseVel;
		Mouse::GetInstance()->GetMouseVelocityX(&mouseVel.x);
		Mouse::GetInstance()->GetMouseVelocityY(&mouseVel.y);
		WCHAR str[64];
		WCHAR str2[64];
		swprintf(str, L"X:%d,Y:%d", p.x, p.y);
		swprintf(str2, L"VX:%f,VY:%f", mouseVel.x, mouseVel.y);
		DWRITE.DrawFormatText(str, Define::WIN_W - 256, Define::WIN_H - 128, 1000, 100, D3D.GetColor(255, 255, 255), 1, 1);
		DWRITE.DrawFormatText(str2, Define::WIN_W - 256, Define::WIN_H - 150, 1000, 100,D3D.GetColor(255,255,255), 1, 1);
		
		D3D.SetDefaultBlendDesc(255);
		D3D.DrawSRVTex(g_fieldSimulation->getResultSRV(), 0, 0, 1024, 1024);
		D3D.DrawBox(0,0, 1024, 1024,D3D.GetColor(30,100,255));
		D3D.DrawSRVTex(g_fieldSimulation->getVelocitySRV(), Define::WIN_W - 256, 0, 256, 256);
		D3D.DrawSRVTex(g_fieldSimulation->getDivergenceSRV(), Define::WIN_W-256, 256, 256, 256);
		D3D.DrawSRVTex(g_fieldSimulation->getPressureSRV(), Define::WIN_W - 256, 512, 256, 256);
		D3D.SetDefaultBlendDesc(255);//D3D.DrawBox(Define::WIN_W/4, Define::WIN_H/4, 300, 300,D3D.GetColor(255,255,0));
		ID3D11ShaderResourceView* nullSRV = nullptr;
		D3D.GetDeviceContext()->PSSetShaderResources(0, 1, &nullSRV);
		DWRITE.Draw2DEnd();//2D描画終了
		D3D.Present(1, 0);
	}


	SAFE_DELETE(g_fieldSimulation);
	finalize();
}

//デバイスなどの解放処理
//入力、描画、音響、デバッグ、ネットなど
void DIRECTOR::finalize()const
{
	DirectInput::DeleteInstance();
	TextureFactory::DeleteInstance();
	DIRECT3D11::DeleteInstance();
	Camera::DeleteInstance();
	Fps::DeleteInstance();
	Log("終了処理は正常に行われました。\n");
}
