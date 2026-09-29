#include "BASE.h"

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

	char line[MAX_PATH];

	//初期値をロード
	/*
	fgets(line, MAX_PATH, fp);
	g_Ini.MeshRender = atoi(line);//1=メッシュを表示 0=非表示

	fgets(line, MAX_PATH, fp);
	g_Ini.SoundPlay = atoi(line);//1=サウンド再生 0=再生しない

	fgets(line, MAX_PATH, fp);
	g_Ini.CameraZ = atof(line);//カメラのZ位置
	*/

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