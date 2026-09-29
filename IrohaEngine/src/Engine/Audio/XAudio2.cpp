#include "XAudio2.h"


XAudio2::~XAudio2()
{
	if (pXAudioMasterVoice != nullptr)
	{
		pXAudioMasterVoice->DestroyVoice();
		pXAudioMasterVoice = nullptr;
	}
	if (pXAudio2 != nullptr)
	{
		pXAudio2->Release();
		pXAudio2 = nullptr;
	}
}

bool XAudio2::Init()
{
	Log("XAudio2の初期化を開始\n");

	CoInitializeEx(nullptr, COINIT_MULTITHREADED);

	if (FAILED(XAudio2Create(&pXAudio2, XAUDIO2_DEBUG_ENGINE)))
	{
		OutputDebugString(L"Xaudio2作成失敗\n");
		return false;
	}
	if (FAILED(pXAudio2->CreateMasteringVoice(&pXAudioMasterVoice)))
	{
		OutputDebugString(L"マスターボイス作成失敗\n");
		return false;
	}
#if defined(_DEBUG)
	XAUDIO2_DEBUG_CONFIGURATION debug{ 0 };
	debug.TraceMask = XAUDIO2_LOG_ERRORS | XAUDIO2_LOG_WARNINGS;
	debug.BreakMask = XAUDIO2_LOG_ERRORS;
	pXAudio2->SetDebugConfiguration(&debug, 0);
#endif
	Log("XAudio2の初期化完了\n");
	return true;
}
