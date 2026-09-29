#include <windows.h>
#include "DIRECTOR.h"

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, INT)
{
	// メモリリーク検出
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF | _CRTDBG_CHECK_EVERY_1024_DF);
	CoInitialize(NULL);
	DIRECTOR director;
	//進行はディレクターに
	if (director.initialize(hInstance))
	{
		director.mainloop();
	}
	CoUninitialize();
	//アプリ終了
	return 0;
}