#include <windows.h>
#include <Iroha.h>
#include "DIRECTOR.h"
HWND hWnd_existed = nullptr;

LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK EnumWindowsProcMy(HWND hwnd, LPARAM lParam);
VOID GetExeOtherProcessIds(CString sTargetExeName, DWORD* dwExeProcessIds, DWORD dwIgnoreProcessId);

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, INT)
{
	// メモリリーク検出
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF | _CRTDBG_CHECK_EVERY_1024_DF);

	// ------------------------------------------------------------------------
   // 二重起動防止
   // ------------------------------------------------------------------------
   // 自分のプロセスIDから自分のexe名を取得
	DWORD dwCurrentProcessId = GetCurrentProcessId();
	HANDLE hProcess = OpenProcess(
		PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
		FALSE,
		dwCurrentProcessId
	);
	TCHAR szModuleName[MAX_PATH];
	GetModuleBaseName(hProcess, NULL, szModuleName, MAX_PATH);

	// 自分のexe名と同じプロセスのプロセスIDを取得(自分のプロセスIDは除く)
	DWORD dwExeProcessIds[1024] = { 0 };
	GetExeOtherProcessIds(szModuleName, dwExeProcessIds, dwCurrentProcessId);

	// 既に起動済みだった場合
	if (0 < dwExeProcessIds[0])
	{
		// プロセスIDからウインドウハンドルを取得
		EnumWindows(EnumWindowsProcMy, dwExeProcessIds[0]);
		if (hWnd_existed)
		{
			// タスクトレイの中に入っている場合、元のサイズに戻す
			if (IsIconic(hWnd_existed))
			{
				ShowWindow(hWnd_existed, SW_RESTORE);
			}
			else
			{
				// 見つかったウィンドウをフォアグラウンドにする
				SetForegroundWindow(GetLastActivePopup(hWnd_existed));
			}
		}
		return FALSE;
	}
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

VOID GetExeOtherProcessIds(CString sTargetExeName, DWORD* dwExeProcessIds, DWORD dwIgnoreProcessId)
{
	DWORD dwAllProcessIds[1024] = { 0 };
	DWORD cbNeeded = 0;
	if (!EnumProcesses(dwAllProcessIds, sizeof(dwAllProcessIds), &cbNeeded))
	{
		return;
	}

	int j = 0;
	int nProc = cbNeeded / sizeof(DWORD);
	for (int i = 0; i < nProc; i++)
	{
		if (dwAllProcessIds[i] == dwIgnoreProcessId)
		{
			continue;
		}

		TCHAR szProcessName[MAX_PATH] = TEXT("<unknown>");

		// Get a handle to the process.
		HANDLE hProcess = OpenProcess(
			PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
			FALSE,
			dwAllProcessIds[i]
		);

		// Get the process name.
		if (hProcess)
		{
			HMODULE hMod;
			DWORD cbNeeded;

			if (EnumProcessModules(hProcess, &hMod, sizeof(hMod),
				&cbNeeded))
			{
				GetModuleBaseName(hProcess, hMod, szProcessName,
					sizeof(szProcessName) / sizeof(TCHAR));

				CString sProcName = CString(szProcessName).MakeUpper();
				if (sProcName == sTargetExeName.MakeUpper())
				{
					dwExeProcessIds[j] = dwAllProcessIds[i];
					++j;
				}
			}

			// Release the handle to the process.
			CloseHandle(hProcess);
		}
	}
}


// プロセスIDからウインドウハンドルを取得
BOOL CALLBACK EnumWindowsProcMy(HWND hwnd, LPARAM lParam)
{
	DWORD lpdwProcessId;
	GetWindowThreadProcessId(hwnd, &lpdwProcessId);
	if (lpdwProcessId == lParam)
	{
		hWnd_existed = hwnd;
		return FALSE;
	}
	return TRUE;
}
