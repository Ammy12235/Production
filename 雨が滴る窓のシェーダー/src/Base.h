#pragma once

#include <stdio.h>

//警告非表示
#pragma warning(disable : 4305)
#pragma warning(disable : 4996)
#pragma warning(disable : 4018)
#pragma warning(disable : 4111)

// Direct3Dのライブラリを使用できるようにする
#pragma comment(lib, "d3d11.lib")
#pragma comment( lib, "d3d9.lib" )
#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib,"dwrite.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"windowscodecs.lib")

//DirectInputのライブラリを使用できるようにする
#pragma comment(lib,"dinput8.lib")

//XInputのライブラリを使用できるようにする
#pragma comment(lib,"xinput.lib")

//DirectSoundのライブラリを使用できるようにする
#pragma comment(lib,"dsound.lib")
#pragma comment(lib,"winmm.lib")

//Xaudio2のライブラリを使用できるようにする
#pragma comment(lib,"Xaudio2.lib")

//Ogg Vorbis等のライブラリを使用できるようにする
/*
#pragma comment ( lib, "libogg_static.lib" )
#pragma comment ( lib, "libvorbis_static.lib" )
#pragma comment ( lib, "libvorbisfile_static.lib" )
*/
#pragma comment(lib, "Mf.lib")
#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "Mfreadwrite.lib")
#pragma comment(lib, "mfuuid.lib")

#include <windows.h>
#include <tchar.h>
#include <wbemidl.h>
#include <oleauto.h>
#include <locale.h>
#include <wincodec.h>
#include <fstream>
#include <sys/stat.h>
#include <map>
#include <string>
#include <stack>
#include <memory>

//=================================================
// 描画群
// ================================================

// Direct3Dの型・クラス・関数などを呼べるようにする
#include <d3d11.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directxcolors.h>
#include <d3d9.h>

// DirectXテクスチャライブラリを使用できるようにする
#include <DirectXTex.h>
#include <wincodec.h>

// DirectXMath(数学ライブラリ)を使用できるようにする
#include <DirectXMath.h>
using namespace DirectX;

//テキスト表示の機能を使用できるようにする
#include <d2d1_1.h>
#include <dwrite_3.h>

//=================================================
// 操作群
// ================================================

//DirectInputを使用できるようにする
#define DIRECTINPUT_VERSION 0x0800		//DirectInputのバージョン指定
#include <dinput.h>

//XInputを使用できるようにする
#include <xinput.h>

//=================================================
// 音響群
// ================================================

//DirectSoundを使用できるようにする
#include <mmsystem.h>//mmio関数群をWinAPIで使用する
#include <dsound.h>

//Xaudio2を使用できるようにする
#include <Xaudio2.h>
#include <Xaudio2fx.h>

//Ogg Vorbis等を使用できるようにする
/*
#include <vorbis/vorbisfile.h>
*/


//=================================================
// デバッグ群
//=================================================
//デバイスメモリーリーク検出用
#include "dxgidebug.h"
//システム情報のログ等
#include "dxdiag.h"
//メモリーリーク検出用
#include <crtdbg.h>

#include <stdlib.h>
#define _CRTDBG_MAP_ALLOC
#ifdef _DEBUG
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
// Replace _NORMAL_BLOCK with _CLIENT_BLOCK if you want the
// allocations to be of _CLIENT_BLOCK type
#else
#define DBG_NEW new
#endif




#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#include <psapi.h>
#include <atlstr.h>




// ComPtrを使用できるようにする
#include <wrl/client.h>
using Microsoft::WRL::ComPtr;

#define SAFE_DELETE(p) { if(p) { delete (p); (p)=NULL; } }
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p); (p)=NULL; } }
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }
#define MFAIL(code,string) if(FAILED( code ) ) { MessageBox(0,string,L"error",MB_OK); return E_FAIL; }
#define MFALSE(code,string) if(!( code ) ) { MessageBox(0,string,L"error",MB_OK); return E_FAIL; }
#define MSG(t) MessageBox(0,t,0,MB_OK);
//
//
//始祖クラス：すべてのクラスが継承する基底クラス。プログラム全体で変更がある場合、ここに書くことで他のすべてを変更できるメリットがある
class CELEMENT
{
};

//
//設定値
struct INI
{
	//初期設定値
	/*
	bool MeshRender;//1=メッシュを表示 0=非表示
	bool SoundPlay;//1=サウンド再生 0=再生しない
	float CameraZ;//カメラのZ位置
	*/

	INI()
	{
		ZeroMemory(this, sizeof(INI));
	}
};

//
//プロトタイプ
void InitDirectory(wchar_t* root);
void SetRootDirectory();
void SetDataDirectory();
void SetVisualDirectory();
void SetShaderDirectory();
void SetSoundDirectory();
HRESULT LoadIni();
INI* GetIni();

void OpenLog();
void Log(const char* str);
HRESULT OutPutDiagLog();