#include "Base.h"

wchar_t g_szRootPath[1124] = { 0 };

INI g_Ini;
FILE* g_Fp = NULL;

//
//
//
void OpenLog()
{
	SetDataDirectory();
	g_Fp = fopen("log.txt", "w");
}
//
//
//ログ記録
void Log(const char* str)
{
	SetDataDirectory();
	g_Fp = fopen("log.txt", "a");
	fwrite(str, sizeof(char), strlen(str), g_Fp);
	fclose(g_Fp);
}

std::wstring ToWide(const std::string& s)
{
	int len = MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, nullptr, 0);
	std::wstring ws(len, L'\0');
	MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, ws.data(), len);
	return ws;
}

//
//
//
INI* GetIni()
{
	return &g_Ini;
}
//
//
//設定パラメーター読み込み
HRESULT LoadIni()
{
	SetDataDirectory();
	FILE* fp = fopen("game.ini", "rt");
	MFALSE(fp, L"初期値のロードに失敗");
	fclose(fp);

	return S_OK;
}

//
//
//
void InitDirectory(wchar_t* root)
{
	wcscpy(g_szRootPath, root);
}

//
//
//
void SetRootDirectory()
{
	SetCurrentDirectory(g_szRootPath);
}
//
//
//
void SetSoundDirectory()
{
	wchar_t tmp[512] = { 0 };
	wcscpy(tmp, g_szRootPath);
	wcscat(tmp, L"\\04 Sound File");
	SetCurrentDirectory(tmp);
}
//
//
//
void SetDataDirectory()
{
	wchar_t tmp[512] = { 0 };
	wcscpy(tmp, g_szRootPath);
	wcscat(tmp, L"\\Data");
	SetCurrentDirectory(tmp);
}
//
//
//
void SetVisualDirectory()
{
	wchar_t tmp[512] = { 0 };
	wcscpy(tmp, g_szRootPath);
	wcscat(tmp, L"\\02 Visual File");
	SetCurrentDirectory(tmp);
}
//
//
//
void SetShaderDirectory()
{
	wchar_t tmp[512] = { 0 };
	wcscpy(tmp, g_szRootPath);
	wcscat(tmp, L"\\Shader");
	SetCurrentDirectory(tmp);
}

HRESULT OutPutDiagLog()
{
	HRESULT hr;
	FILE* f = NULL;
	IDxDiagProvider* pDxDiagProvider = NULL;
	IDxDiagContainer* pDxDiagRoot = NULL;
	IDxDiagContainer* pDxChild = NULL;
	char str[100], strDestination[100];
	VARIANT var;

	hr = CoInitialize(NULL);
	if (FAILED(hr))
		return hr;

	hr = CoCreateInstance(CLSID_DxDiagProvider,
		NULL,
		CLSCTX_INPROC_SERVER,
		IID_IDxDiagProvider,
		(LPVOID*)&pDxDiagProvider);
	if (SUCCEEDED(hr))
	{
		DXDIAG_INIT_PARAMS dxDiagInitParam;
		ZeroMemory(&dxDiagInitParam, sizeof(DXDIAG_INIT_PARAMS));
		dxDiagInitParam.dwSize = sizeof(DXDIAG_INIT_PARAMS);
		dxDiagInitParam.dwDxDiagHeaderVersion = DXDIAG_DX9_SDK_VERSION;
		dxDiagInitParam.bAllowWHQLChecks = false;
		dxDiagInitParam.pReserved = NULL;

		hr = pDxDiagProvider->Initialize(&dxDiagInitParam);
		if (SUCCEEDED(hr))
		{
			//ルート コンテナにする IDxDiagContainer オブジェクトを作成し、初期化する。
			hr = pDxDiagProvider->GetRootContainer(&pDxDiagRoot);
			if (SUCCEEDED(hr))
			{
				//****************************************************************         
				//DirectX診断ツールのシステムタブの情報を取得する。
				//****************************************************************         
				hr = pDxDiagRoot->GetChildContainer(L"DxDiag_SystemInfo", &pDxChild);
				if (SUCCEEDED(hr))
				{
					Log("--- システム ---\n");

					//****************************************************************
					//OSバージョン
					//****************************************************************
					VariantInit(&var);
					hr = pDxChild->GetProp(L"szOSExLocalized", &var);
					if (SUCCEEDED(hr) && var.vt == VT_BSTR && SysStringLen(var.bstrVal) != 0)
					{
						WideCharToMultiByte(CP_ACP, 0, var.bstrVal, -1, strDestination, 100 * sizeof(CHAR), NULL, NULL);
						sprintf(str, "         OS： %s\n", strDestination);
						Log(str);
					}
					VariantClear(&var);

					//****************************************************************
					//CPU
					//****************************************************************
					VariantInit(&var);
					hr = pDxChild->GetProp(L"szProcessorEnglish", &var);
					if (SUCCEEDED(hr) && var.vt == VT_BSTR && SysStringLen(var.bstrVal) != 0)
					{
						WideCharToMultiByte(CP_ACP, 0, var.bstrVal, -1, strDestination, 100 * sizeof(CHAR), NULL, NULL);
						sprintf(str, "        CPU： %s\n", strDestination);
						Log(str);
					}
					VariantClear(&var);

					//****************************************************************
					//SystemMemoryの容量
					//****************************************************************
					VariantInit(&var);
					hr = pDxChild->GetProp(L"szPhysicalMemoryEnglish", &var);
					if (SUCCEEDED(hr) && var.vt == VT_BSTR && SysStringLen(var.bstrVal) != 0)
					{
						WideCharToMultiByte(CP_ACP, 0, var.bstrVal, -1, strDestination, 100 * sizeof(CHAR), NULL, NULL);
						sprintf(str, "  SysMemory： %s\n", strDestination);
						Log(str);
					}
					VariantClear(&var);

					//****************************************************************
					//DirectXランタイムのバージョン
					//****************************************************************
					VariantInit(&var);
					hr = pDxChild->GetProp(L"szDirectXVersionLongEnglish", &var);
					if (SUCCEEDED(hr) && var.vt == VT_BSTR && SysStringLen(var.bstrVal) != 0)
					{
						WideCharToMultiByte(CP_ACP, 0, var.bstrVal, -1, strDestination, 100 * sizeof(CHAR), NULL, NULL);
						sprintf(str, "    DirectX： %s\n", strDestination);
						Log(str);
					}
					VariantClear(&var);

					//****************************************************************
					//DirectXMode
					//****************************************************************
					VariantInit(&var);
					hr = pDxChild->GetProp(L"bIsD3DDebugRuntime", &var);
					if (SUCCEEDED(hr) && var.vt == VT_BOOL)
					{
						if (var.boolVal == TRUE)
							sprintf(str, "DirectXMode： DebugRuntime\n");
						else
							sprintf(str, "DirectXMode： RetailRuntime\n");
						Log(str);
					}
					VariantClear(&var);

					pDxChild->Release();
				}

				//****************************************************************
				//ディスプレイタブの情報を取得する。
				//****************************************************************
				hr = pDxDiagRoot->GetChildContainer(L"DxDiag_DisplayDevices", &pDxChild);
				if (SUCCEEDED(hr))
				{
					DWORD DisplayCnt;
					WCHAR wszContainer[100];
					IDxDiagContainer* pDxDisplay = NULL;

					//ディスプレイアダプタの数を取得する。
					pDxChild->GetNumberOfChildContainers(&DisplayCnt);

					for (DWORD i = 0; i < DisplayCnt; i++)
					{
						sprintf(str, "--- ディスプレイアダプタ%d ---\n", i);
						Log(str);

						//ディスプレイの列挙
						hr = pDxChild->EnumChildContainerNames(i, wszContainer, 100);
						if (SUCCEEDED(hr))
						{
							hr = pDxChild->GetChildContainer(wszContainer, &pDxDisplay);
							if (SUCCEEDED(hr))
							{
								//****************************************************************
								//チップの種類
								//****************************************************************
								VariantInit(&var);
								hr = pDxDisplay->GetProp(L"szChipType", &var);
								if (SUCCEEDED(hr) && var.vt == VT_BSTR && SysStringLen(var.bstrVal) != 0)
								{
									WideCharToMultiByte(CP_ACP, 0, var.bstrVal, -1, strDestination, 100 * sizeof(CHAR), NULL, NULL);
									sprintf(str, "       Chip： %s\n", strDestination);
									Log(str);
								}
								VariantClear(&var);

								//****************************************************************
								//VRAM
								//****************************************************************
								VariantInit(&var);
								hr = pDxDisplay->GetProp(L"szDisplayMemoryEnglish", &var);
								if (SUCCEEDED(hr) && var.vt == VT_BSTR && SysStringLen(var.bstrVal) != 0)
								{
									WideCharToMultiByte(CP_ACP, 0, var.bstrVal, -1, strDestination, 100 * sizeof(CHAR), NULL, NULL);
									sprintf(str, "       VRAM： %s\n", strDestination);
									Log(str);
								}
								VariantClear(&var);

								//****************************************************************
								//DDI
								//****************************************************************
								VariantInit(&var);
								hr = pDxDisplay->GetProp(L"szDDIVersionEnglish", &var);
								if (SUCCEEDED(hr) && var.vt == VT_BSTR && SysStringLen(var.bstrVal) != 0)
								{
									WideCharToMultiByte(CP_ACP, 0, var.bstrVal, -1, strDestination, 100 * sizeof(CHAR), NULL, NULL);
									sprintf(str, "        DDI： %s\n", strDestination);
									Log(str);
								}
								VariantClear(&var);

								//****************************************************************
								//DirectDrawアクセラレータ
								//****************************************************************
								VariantInit(&var);
								hr = pDxDisplay->GetProp(L"bDDAccelerationEnabled", &var);
								if (SUCCEEDED(hr) && var.vt == VT_BOOL)
								{
									if (var.boolVal == TRUE)
										sprintf(str, " DirectDraw： 使用可能\n");
									else
										sprintf(str, " DirectDraw： 無効\n");
									Log(str);
								}
								VariantClear(&var);

								//****************************************************************
								//Direct3Dアクセラレータ
								//****************************************************************
								VariantInit(&var);
								hr = pDxDisplay->GetProp(L"b3DAccelerationEnabled", &var);
								if (SUCCEEDED(hr) && var.vt == VT_BOOL)
								{
									if (var.boolVal == TRUE)
										sprintf(str, "   Direct3D： 使用可能\n");
									else
										sprintf(str, "   Direct3D： 無効\n");
									Log(str);
								}
								VariantClear(&var);

								//****************************************************************
								//AGPアクセラレータ
								//****************************************************************
								VariantInit(&var);
								hr = pDxDisplay->GetProp(L"bAGPEnabled", &var);
								if (SUCCEEDED(hr) && var.vt == VT_BOOL)
								{
									if (var.boolVal == TRUE)
										sprintf(str, "        AGP： 使用可能\n");
									else
										sprintf(str, "        AGP： 無効\n");
									Log(str);
								}
								VariantClear(&var);

								pDxDisplay->Release();
							}
						}
					}
					pDxChild->Release();
				}
				pDxDiagRoot->Release();
			}
		}
		pDxDiagProvider->Release();
	}

	CoUninitialize();
}
