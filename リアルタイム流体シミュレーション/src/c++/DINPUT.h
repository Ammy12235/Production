#pragma once

#include "BASE.h"

#define RANGE 1000
#define DIDEVICE_BUFFERSIZE 100
#define THRESHOLD RANGE/4
#define PAD_NUM 1

//ダイレクトインプットクラス：入力に使用するDirectInputデバイスの初期化、更新、削除を行うクラス。
class DirectInput :public CELEMENT
{
public:


	//DirectInput
	LPDIRECTINPUT8 lpDi = nullptr;
	//DirectInputキーボードデバイス
	LPDIRECTINPUTDEVICE8 lpKeyBoard = nullptr;
	//DirectInputマウスデバイス
	LPDIRECTINPUTDEVICE8 lpMouse = nullptr;
	//DirectInputパッドデバイス
	LPDIRECTINPUTDEVICE8 lpJoyPad = nullptr;
	//マウスのデータ
	DIMOUSESTATE2 dim;
	//ジョイスティックの能力
	DIDEVCAPS DevCaps;
	//ジョイパッドのデータ
	DIJOYSTATE2 dij;


	LPDIRECTINPUTEFFECT pDIEffect; // エフェクトオブジェクトへのインタフェース

	DWORD g_dwNumForceFeedbackAxis;

	//DirectInputの初期化
	bool Initialize(HWND hWnd, HINSTANCE hInstance);
	//DirectInputキーボードの初期化
	bool Init_KeyBoard(HWND hWnd);
	//DirectInputマウスの初期化
	bool Init_Mouse(HWND hWnd);
	//DirectInputジョイパッドの初期化
	bool Init_JoyPad(HWND hwnd);
	bool Init_ForceFeedback_JoyPad(HWND hwnd);
	BOOL IsXInputDevice(const GUID* pGuidProductFromDirectInput);

	HRESULT CreateEffect(HWND hWnd);

	void CheckHitKey(char* key);
	int CheckHitPad();
	int CheckHitMouse();

	//排他制御によるフォーカスが外れた時に再び取得する
	void Re_Input();

	//デバイスを再び初期化
	void Re_Init();


	//キーの入力状態を格納する変数
	BYTE key[256];
	//DirectInput内で使いまわすウインドウハンドル
	HWND hWnd;

private:

	//デバイス列挙のためのコールバック関数
	static BOOL PASCAL EnumJoySticksCallBack(const DIDEVICEINSTANCE* pdidInstance, VOID* pContext);
	static BOOL PASCAL EnumAxesCallback(const DIDEVICEOBJECTINSTANCE* pddoi, VOID* pContext);
	static BOOL PASCAL EnumEffectCallBack(LPCDIEFFECTINFO pdei, LPVOID pvRef);

	//デバイスのメモリ解放
	bool ReleaseInputDevice();

	DirectInput()
	{
		ZeroMemory(key, sizeof(key));
		ZeroMemory(&dim, sizeof(DIMOUSESTATE2));
	}
	~DirectInput()
	{
		ReleaseInputDevice();
	}

	static inline DirectInput* s_instance;
public:
	static void CreateInstance()
	{
		DeleteInstance();

		s_instance = new DirectInput();
	}

	static void DeleteInstance()
	{
		if (s_instance != nullptr)
		{
			delete s_instance;
			s_instance = nullptr;
		}
	}

	static DirectInput& GetInstance()
	{
		return *s_instance;
	}
protected:



};

#define DINPUT DirectInput::GetInstance()