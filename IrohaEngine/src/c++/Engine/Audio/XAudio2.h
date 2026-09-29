#pragma once 

#include "Core/Singleton.h"

class XAudio2:public Singleton<XAudio2>
{
private:
	//Xaudio2
	IXAudio2* pXAudio2 = nullptr;
	//Xaudio2マスタ―ボイス
	IXAudio2MasteringVoice* pXAudioMasterVoice = nullptr;
	XAUDIO2_VOICE_STATE state;

public:

	bool Init();
	IXAudio2* GetXAudio2Device()const
	{
		return pXAudio2;
	}

	IXAudio2MasteringVoice* GetXAudo2MasterVoice()const
	{
		return pXAudioMasterVoice;
	}
	XAudio2()=default;

	~XAudio2();

private:




};

