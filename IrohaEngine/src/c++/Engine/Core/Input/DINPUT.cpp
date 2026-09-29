#include "DINPUT.h"
#include "Core/DEFINE.h"

int i = 0;

BOOL PASCAL DirectInput::EnumAxesCallback(const DIDEVICEOBJECTINSTANCE* pddoi, VOID* pContext)
{
	if (pddoi->dwType & DIDFT_AXIS)
	{
		DIPROPRANGE diprg;
		ZeroMemory(&diprg, sizeof(diprg));
		diprg.diph.dwSize = sizeof(diprg);
		diprg.diph.dwHeaderSize = sizeof(diprg.diph);
		diprg.diph.dwObj = pddoi->dwType;
		diprg.diph.dwHow = DIPH_BYID;
		diprg.lMin = -RANGE;
		diprg.lMax = RANGE;

		if (FAILED(s_instance->lpJoyPad->SetProperty(DIPROP_RANGE, &diprg.diph)))
		{
			OutputDebugString(L"プロパティ設定失敗\n");
			return DIENUM_STOP;
		}
	}
	DWORD* pdwNumForceFeedbackAxis = (DWORD*)pContext;
	if ((pddoi->dwFlags & DIDOI_FFACTUATOR) != 0) (*pdwNumForceFeedbackAxis)++;

	return DIENUM_CONTINUE;
}

BOOL PASCAL DirectInput::EnumJoySticksCallBack(const DIDEVICEINSTANCE* pdidInstance, VOID* pContext)
{
	/*
	WCHAR szConfirm[MAX_PATH];

	wsprintf(szConfirm, L"このデバイスでデバイスオブジェクトを作成しますか？\n%s\n%s", pdidInstance->tszProductName, pdidInstance->tszInstanceName);
	if (MessageBox(0, szConfirm, L"確認", MB_YESNO) == IDNO)
	{
		return DIENUM_CONTINUE;
	}
	*/
	if (s_instance->IsXInputDevice(&pdidInstance->guidProduct))
	{
		OutputDebugString(L"Xinputデバイスが検知されました\n");
		return DIENUM_CONTINUE;

	}

	if (FAILED(s_instance->lpDi->CreateDevice(pdidInstance->guidInstance, &s_instance->lpJoyPad, NULL)))
	{
		OutputDebugString(L"パッドデバイス作成失敗\n");
		return DIENUM_CONTINUE;
	}

	return DIENUM_CONTINUE;
}

BOOL PASCAL DirectInput::EnumEffectCallBack(LPCDIEFFECTINFO pdei, LPVOID pvRef)
{
	std::vector<DIEFFECTINFO>* pvecEffectInfo = (std::vector<DIEFFECTINFO>*) pvRef;
	pvecEffectInfo->push_back(*pdei);

	return DIENUM_STOP;
}


//初期化
bool DirectInput::Initialize(HWND hWnd, HINSTANCE hInstance)
{
	DirectInput::hWnd = hWnd;
	Log("DirectInputの初期化を開始\n");
	//DirectInputオブジェクト作成
	if (FAILED(DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (LPVOID*)&lpDi, nullptr)))
	{
		OutputDebugString(L"DINPUTオブジェクト作成失敗\n");
		return false;
	}

	DirectInput::Init_KeyBoard(hWnd);
	DirectInput::Init_Mouse(hWnd);
	DirectInput::Init_ForceFeedback_JoyPad(hWnd);
	DirectInput::Init_JoyPad(hWnd);

	Log("DirecInputの初期化完了\n");
	return true;
}

BOOL DirectInput::IsXInputDevice(const GUID* pGuidProductFromDirectInput)
{
	IWbemLocator* pIWbemLocator = nullptr;
	IEnumWbemClassObject* pEnumDevices = nullptr;
	IWbemClassObject* pDevices[20] = {};
	IWbemServices* pIWbemServices = nullptr;
	BSTR                    bstrNamespace = nullptr;
	BSTR                    bstrDeviceID = nullptr;
	BSTR                    bstrClassName = nullptr;
	bool                    bIsXinputDevice = false;

	// CoInit if needed
	HRESULT hr = CoInitialize(nullptr);
	bool bCleanupCOM = SUCCEEDED(hr);

	// So we can call VariantClear() later, even if we never had a successful IWbemClassObject::Get().
	VARIANT var = {};
	VariantInit(&var);

	// Create WMI
	hr = CoCreateInstance(__uuidof(WbemLocator),
		nullptr,
		CLSCTX_INPROC_SERVER,
		__uuidof(IWbemLocator),
		(LPVOID*)&pIWbemLocator);
	if (FAILED(hr) || pIWbemLocator == nullptr)
		goto LCleanup;

	bstrNamespace = SysAllocString(L"\\\\.\\root\\cimv2");  if (bstrNamespace == nullptr) goto LCleanup;
	bstrClassName = SysAllocString(L"Win32_PNPEntity");     if (bstrClassName == nullptr) goto LCleanup;
	bstrDeviceID = SysAllocString(L"DeviceID");             if (bstrDeviceID == nullptr)  goto LCleanup;

	// Connect to WMI 
	hr = pIWbemLocator->ConnectServer(bstrNamespace, nullptr, nullptr, 0L,
		0L, nullptr, nullptr, &pIWbemServices);
	if (FAILED(hr) || pIWbemServices == nullptr)
		goto LCleanup;

	// Switch security level to IMPERSONATE. 
	hr = CoSetProxyBlanket(pIWbemServices,
		RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, nullptr,
		RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE,
		nullptr, EOAC_NONE);
	if (FAILED(hr))
		goto LCleanup;

	hr = pIWbemServices->CreateInstanceEnum(bstrClassName, 0, nullptr, &pEnumDevices);
	if (FAILED(hr) || pEnumDevices == nullptr)
		goto LCleanup;

	// Loop over all devices
	for (;;)
	{
		ULONG uReturned = 0;
		hr = pEnumDevices->Next(10000, _countof(pDevices), pDevices, &uReturned);
		if (FAILED(hr))
			goto LCleanup;
		if (uReturned == 0)
			break;

		for (size_t iDevice = 0; iDevice < uReturned; ++iDevice)
		{
			// For each device, get its device ID
			hr = pDevices[iDevice]->Get(bstrDeviceID, 0L, &var, nullptr, nullptr);
			if (SUCCEEDED(hr) && var.vt == VT_BSTR && var.bstrVal != nullptr)
			{
				// Check if the device ID contains "IG_".  If it does, then it's an XInput device
				// This information can not be found from DirectInput 
				if (wcsstr(var.bstrVal, L"IG_"))
				{
					// If it does, then get the VID/PID from var.bstrVal
					DWORD dwPid = 0, dwVid = 0;
					WCHAR* strVid = wcsstr(var.bstrVal, L"VID_");
					if (strVid && swscanf_s(strVid, L"VID_%4X", &dwVid) != 1)
						dwVid = 0;
					WCHAR* strPid = wcsstr(var.bstrVal, L"PID_");
					if (strPid && swscanf_s(strPid, L"PID_%4X", &dwPid) != 1)
						dwPid = 0;

					// Compare the VID/PID to the DInput device
					DWORD dwVidPid = MAKELONG(dwVid, dwPid);
					if (dwVidPid == pGuidProductFromDirectInput->Data1)
					{
						bIsXinputDevice = true;
						goto LCleanup;
					}
				}
			}
			VariantClear(&var);
			SAFE_RELEASE(pDevices[iDevice]);
		}
	}

LCleanup:
	VariantClear(&var);

	if (bstrNamespace)
		SysFreeString(bstrNamespace);
	if (bstrDeviceID)
		SysFreeString(bstrDeviceID);
	if (bstrClassName)
		SysFreeString(bstrClassName);

	for (size_t iDevice = 0; iDevice < _countof(pDevices); ++iDevice)
		SAFE_RELEASE(pDevices[iDevice]);

	SAFE_RELEASE(pEnumDevices);
	SAFE_RELEASE(pIWbemLocator);
	SAFE_RELEASE(pIWbemServices);

	if (bCleanupCOM)
		CoUninitialize();

	return bIsXinputDevice;
}

HRESULT DirectInput::CreateEffect(HWND hWnd)
{
	HRESULT hr;
	//エフェクト周期設定
	DIPERIODIC diprd;
	ZeroMemory(&diprd, sizeof(diprd));
	diprd.dwMagnitude = 10000;
	diprd.lOffset = 0;
	diprd.dwPhase = 0;
	diprd.dwPeriod = (DWORD)(DI_SECONDS * 0.5f);

	// 振動エフェクト設定
	DWORD Axes[] = { DIJOFS_X, DIJOFS_Y }; // エフェクト軸
	LONG Direction[] = { 0, 0 }; // エフェクト方向

	DIEFFECT Effect;
	ZeroMemory(&Effect, sizeof(Effect));
	Effect.dwSize = sizeof(Effect);
	Effect.dwFlags = DIEFF_POLAR | DIEFF_OBJECTOFFSETS;
	Effect.dwDuration = (DWORD)(0.5 * DI_SECONDS); // エフェクト継続時間
	Effect.dwSamplePeriod = 0;
	Effect.dwGain = DI_FFNOMINALMAX;
	Effect.dwTriggerButton = DIEB_NOTRIGGER;
	Effect.dwTriggerRepeatInterval = 0;
	Effect.cAxes = sizeof(Axes) / sizeof(Axes[0]); // 配列のサイズ
	Effect.rgdwAxes = Axes;
	Effect.rglDirection = Direction;
	Effect.lpEnvelope = NULL; // エンベロープ構造体
	Effect.cbTypeSpecificParams = sizeof(diprd); // エフェクト周期構造体のサイズ
	Effect.lpvTypeSpecificParams = &diprd; // エフェクト周期構造体

	// エフェクト生成(pDIDGamePadはフォースフィードバック対応の初期化済みデバイスオブジェクト)

	hr = lpJoyPad->CreateEffect(GUID_Square, &Effect, &pDIEffect, NULL);
	if (FAILED(hr))return hr;

	return S_OK;
}

bool DirectInput::Init_ForceFeedback_JoyPad(HWND hWnd)
{
	if (lpJoyPad != nullptr)
	{
		lpJoyPad->Unacquire();
		lpJoyPad->Release();
		lpJoyPad = nullptr;
	}

	if (FAILED(lpDi->EnumDevices(DI8DEVCLASS_GAMECTRL, EnumJoySticksCallBack, NULL, DIEDFL_FORCEFEEDBACK | DIEDFL_ATTACHEDONLY)) || !lpJoyPad)
	{
		OutputDebugString(L"フィードバック機能付きデバイス列挙失敗\n");
		return false;
	}

	//フォーマット設定
	if (FAILED(lpJoyPad->SetDataFormat(&c_dfDIJoystick2)))
	{
		OutputDebugString(L"DINPUTパッドフォーマット設定失敗\n");
		return false;
	}

	//協調レベル設定
	if (FAILED(lpJoyPad->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE | DISCL_NOWINKEY)))
	{
		OutputDebugString(L"DINPUTパッド協調レベル設定失敗\n");
		return false;
	}

	//軸モードの列挙
	if (FAILED(lpJoyPad->EnumObjects(EnumAxesCallback, (VOID*)&g_dwNumForceFeedbackAxis, DIDFT_ALL)))
	{
		OutputDebugString(L"パッドの軸列挙失敗\n");
		return false;
	}
	if (g_dwNumForceFeedbackAxis > 2) g_dwNumForceFeedbackAxis = 2;

	if (FAILED(CreateEffect(hWnd))) {
		OutputDebugString(L"エフェクト作成失敗\n");
		return FALSE;
	}

	if (lpJoyPad != nullptr)
	{
		//入力状態を取得開始
		lpJoyPad->Acquire();
		lpJoyPad->Poll();

	}

	return true;
}

bool DirectInput::Init_JoyPad(HWND hWnd)
{
	for (i = 0; i < PAD_NUM; i++)
	{
		if (lpJoyPad != nullptr)
		{
			lpJoyPad->Unacquire();
			lpJoyPad->Release();
			lpJoyPad = nullptr;
		}

		//デバイス列挙
		if (FAILED(lpDi->EnumDevices(DI8DEVCLASS_GAMECTRL, EnumJoySticksCallBack, NULL, DIEDFL_ATTACHEDONLY)) || !lpJoyPad)
		{
			OutputDebugString(L"デバイス列挙失敗\n");
			return false;
		}

		//フォーマット設定
		if (FAILED(lpJoyPad->SetDataFormat(&c_dfDIJoystick2)))
		{
			OutputDebugString(L"DINPUTパッドフォーマット設定失敗\n");
			return false;
		}

		//協調レベル設定
		if (FAILED(lpJoyPad->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY)))
		{
			OutputDebugString(L"DINPUTパッド協調レベル設定失敗\n");
			return false;
		}

		//軸モードの列挙
		if (FAILED(lpJoyPad->EnumObjects(EnumAxesCallback, NULL, DIDFT_ALL)))
		{
			OutputDebugString(L"パッドの軸列挙失敗\n");
			return false;
		}

		if (lpJoyPad != nullptr)
		{
			//入力状態を取得開始
			lpJoyPad->Acquire();
			lpJoyPad->Poll();
		}
	}
	return true;

}

bool DirectInput::Init_KeyBoard(HWND hWnd)
{
	if (lpKeyBoard != nullptr)
	{
		lpKeyBoard->Unacquire();
		lpKeyBoard->Release();
		lpKeyBoard = nullptr;
	}
	Log("     キーボードデバイスの初期化...");
	//キーボードの取得
	if (FAILED(lpDi->CreateDevice(GUID_SysKeyboard, &lpKeyBoard, NULL)))
	{
		OutputDebugString(L"キーボードの取得失敗\n");
		Log("失敗\n");
		return false;
	}

	//フォーマット設定
	if (FAILED(lpKeyBoard->SetDataFormat(&c_dfDIKeyboard)))
	{
		OutputDebugString(L"DINPUTキーボードフォーマット設定失敗\n");
		lpKeyBoard->Release();
		lpDi->Release();
		return false;
	}

	//協調レベル設定
	if (FAILED(lpKeyBoard->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY)))
	{
		OutputDebugString(L"DINPUTキーボード協調レベル設定失敗\n");
		lpKeyBoard->Release();
		lpDi->Release();
		return false;
	}
	Log("成功\n");
	//入力情報を取得開始
	lpKeyBoard->Acquire();

	return true;
}

bool DirectInput::Init_Mouse(HWND hWnd)
{
	if (lpMouse != nullptr)
	{
		lpMouse->Unacquire();
		lpMouse->Release();
		lpMouse = nullptr;
	}
	Log("     マウスデバイスの初期化...");
	//マウスの取得
	if (FAILED(lpDi->CreateDevice(GUID_SysMouse, &lpMouse, NULL)))
	{
		OutputDebugString(L"マウスの取得失敗\n"); Log("失敗\n");
		return false;
	}

	//フォーマット設定
	if (FAILED(lpMouse->SetDataFormat(&c_dfDIMouse2)))
	{
		OutputDebugString(L"DINPUTマウスフォーマット設定失敗\n");
		lpMouse->Release();
		lpDi->Release();
		return false;
	}

	//協調レベル設定
	if (FAILED(lpMouse->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY)))
	{
		OutputDebugString(L"DINPUTマウス協調レベル設定失敗\n");
		lpMouse->Release();
		lpDi->Release();
		return false;
	}
	Log("成功\n");
	//入力情報を取得開始
	lpMouse->Acquire();

	return true;
}

//キー入力を取得
void DirectInput::CheckHitKey(char* key)
{
	lpKeyBoard->GetDeviceState(sizeof(DirectInput::key), DirectInput::key);
	for (int i = 0; i < 256; i++)
	{
		key[i] = DirectInput::key[i];
	}

}

//パッド入力状態を取得
int DirectInput::CheckHitPad()
{
	HRESULT hr=S_OK;
	if (SUCCEEDED(hr))
	{
		//*Nintendo ProController参照
		unsigned int flag = 0;
		if (dij.rgbButtons[0] & 0x80)flag |= (1 << 7);//B
		if (dij.rgbButtons[1] & 0x80)flag |= (1 << 6);//A
		if (dij.rgbButtons[2] & 0x80)flag |= (1 << 8);//X
		if (dij.rgbButtons[3] & 0x80)flag |= (1 << 9);//Y

		if (dij.rgbButtons[4] & 0x80)flag |= (1 << 4);//L
		if (dij.rgbButtons[5] & 0x80)flag |= (1 << 5);//R
		if (dij.rgbButtons[6] & 0x80)flag |= (1 << 14);//ZL
		if (dij.rgbButtons[7] & 0x80)flag |= (1 << 15);//ZR

		if (dij.rgbButtons[8] & 0x80)flag |= (1 << 11);//マイナス
		if (dij.rgbButtons[9] & 0x80)flag |= (1 << 10);//プラス
		if (dij.rgbButtons[10] & 0x80)flag |= (1 << 12);//押し込みひだり
		if (dij.rgbButtons[11] & 0x80)flag |= (1 << 13);//押し込みみぎ

		return flag; 
	}
	else return 0;
}

//マウス入力状態を取得
int DirectInput::CheckHitMouse()
{
	HRESULT hr = S_OK;
	if (SUCCEEDED(hr))
	{
		unsigned int flag = 0;
		if (dim.rgbButtons[0] & 0x80)flag |= (1 << 0);//ひだり
		if (dim.rgbButtons[1] & 0x80)flag |= (1 << 1);//みぎ
		if (dim.rgbButtons[2] & 0x80)flag |= (1 << 2);//真ん中

		return flag;
	}
	else return 0;

}

//排他制御によるフォーカスが外れた時に再び取得する
void DirectInput::Re_Input()
{
	if (lpKeyBoard != nullptr)
	{
		if (FAILED(lpKeyBoard->GetDeviceState(sizeof(key), key)))
		{
			lpKeyBoard->Acquire();
			lpKeyBoard->GetDeviceState(sizeof(key), key);
		}
	}

	if (lpMouse != nullptr)
	{
		if (FAILED(lpMouse->GetDeviceState(sizeof(DIMOUSESTATE2), &dim)))
		{
			lpMouse->Acquire();
			lpMouse->GetDeviceState(sizeof(DIMOUSESTATE2), &dim);
		}
	}

	for (int i = 0; i < PAD_NUM; i++)
	{
		if (lpJoyPad != nullptr)
		{
			if (FAILED(lpJoyPad->GetDeviceState(sizeof(DIJOYSTATE2), &dij)))
			{
				lpJoyPad->Acquire();
				lpJoyPad->GetDeviceState(sizeof(DIJOYSTATE2), &dij);
			}
		}
	}
}

void DirectInput::Re_Init()
{
	for (int i = 0; i < PAD_NUM; i++)
	{
		if (lpJoyPad == nullptr)
		{
			Init_ForceFeedback_JoyPad(DirectInput::hWnd);
			Init_JoyPad(DirectInput::hWnd);
		}
	}
}

//終了処理（メモリ解放）
bool DirectInput::ReleaseInputDevice()
{
	if (pDIEffect != nullptr)
	{
		// エフェクトオブジェクトの解放
		pDIEffect->Unload(); // アンロード
		pDIEffect->Release(); // 解放
	}
	if (lpKeyBoard != nullptr)
	{
		lpKeyBoard->Unacquire();
		lpKeyBoard->Release();
		lpKeyBoard = nullptr;
	}

	if (lpMouse != nullptr)
	{
		lpMouse->Unacquire();
		lpMouse->Release();
		lpMouse = nullptr;
	}

	for (int i = 0; i < PAD_NUM; i++)
	{

		if (lpJoyPad != nullptr)
		{
			lpJoyPad->Unacquire();
			lpJoyPad->Release();
			lpJoyPad = nullptr;
		}
	}
	lpDi->Release();
	lpDi = nullptr;

	return true;
}






