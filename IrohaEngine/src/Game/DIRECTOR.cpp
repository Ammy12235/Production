#include <Iroha.h>
#include "DIRECTOR.h"

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


	//エンジンの初期化
	EngineLoop::CreateInstance();
	EngineLoop::GetInstance().InitializeEngine(hInstance);

	Log("初期化処理は正常に行われました\n");
	return true;
}

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
		
		if (EngineLoop::GetInstance().ExecuteEngineLoop()==false)
		{
			break;
		}
		
	}

	finalize();
}

//デバイスなどの解放処理
//入力、描画、音響、デバッグ、ネットなど
void DIRECTOR::finalize()const
{
	//エンジンの終了処理
	EngineLoop::GetInstance().FinalizeEngine();
	EngineLoop::DeleteInstance();

}
