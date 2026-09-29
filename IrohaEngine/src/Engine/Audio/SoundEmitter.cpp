#include "SoundEmitter.h"
#include "XAudio2.h"
#include "SoundFactory.h"
SoundEmitter::SoundEmitter()
{
	for (int i = 0; i < sourceVoice.size(); i++)
	{
		sourceVoice[i] = nullptr;
	}
}

SoundEmitter::~SoundEmitter()
{
	for (int i = 0; i < sourceVoice.size(); i++)
	{
		if (sourceVoice[i] != nullptr)
		{
			sourceVoice[i]->Stop(0);
			sourceVoice[i]->FlushSourceBuffers();
			sourceVoice[i]->DestroyVoice();
		}
		sourceVoice[i] = nullptr;
	}
}
void SoundEmitter::Update()
{

}

void SoundEmitter::SetVolume(float volume)
{
	_volume = volume;
	for (size_t i = 0; i < sourceVoice.size(); i++)
	{
		if (sourceVoice[i] != nullptr)
		{
			sourceVoice[i]->SetVolume(_volume);
		}
	}
}

void SoundEmitter::SetPan(float pan)
{
	/*
	_pan = pan;

	// pan of -1.0 indicates all left speaker,
// 1.0 is all right speaker, 0.0 is split between left and right
	float left = 0.5f - pan / 2;
	float right = 0.5f + pan / 2;


	XAUDIO2_VOICE_DETAILS details;
	XAudio2::GetInstance().GetXAudo2MasterVoice()->GetVoiceDetails(&details);

	UINT dstChannels = details.InputChannels;

	float matrix[16] = {};

	// source L
	matrix[0] = left; // FL
	matrix[1] = right; // FR

	// source R
	matrix[8] = right;
	matrix[9] = left;

	for (int i = 0; i < sourceVoice.size(); i++)
	{
		sourceVoice[i]->SetOutputMatrix(
			NULL, 2, details.InputChannels, matrix
		);
	}
	*/


}

void SoundEmitter::Play(int id, bool loopFlag)
{
	std::shared_ptr<Sound> sound = SND_FAC.getSoundbyId(id);//IDからサウンドのポインターを取得
	if (!sound)return;
	if (isFirstPlay)
	{
		for (size_t i = 0; i < sourceVoice.size(); i++)
		{
			sourceVoice[i] = nullptr;
			HRESULT hr = XAudio2::GetInstance().GetXAudio2Device()->CreateSourceVoice(&sourceVoice[i], &sound->getWaveFormat());//事前に最大再生数だけソースボイスを作っておく
			if (FAILED(hr))
			{
				sourceVoice[i] = nullptr;
			}
		}
		isFirstPlay = false;
	}
	int freeIndex = SearchFreeSourceVoice();//空きを探す。
	if (freeIndex == -1)return;//もし、空きがなければ、音を出さない。
	else if (sourceVoice[freeIndex] == nullptr) return;//空いててもそれが作られていなかったら

	//XAudio2のソースボイスを設定し、マスターボイスに登録する
	XAUDIO2_BUFFER buffer{ 0 };
	buffer.pAudioData = sound->getSoundData().data();
	buffer.Flags = XAUDIO2_END_OF_STREAM;
	buffer.AudioBytes = sound->getSoundData().size();
	if (loopFlag == true)
	{
		buffer.LoopCount = XAUDIO2_LOOP_INFINITE;//ループフラグがオンなら循環させる
	}
	HRESULT hr = sourceVoice[freeIndex]->SubmitSourceBuffer(&buffer);
	if (FAILED(hr))return;

	hr = sourceVoice[freeIndex]->Start(0, XAUDIO2_COMMIT_NOW);
	if (FAILED(hr))return;
}

void SoundEmitter::Stop()
{
	for (int i = 0; i < sourceVoice.size(); i++)
	{
		if (sourceVoice[i] != nullptr)
		{
			sourceVoice[i]->Stop(0);
			sourceVoice[i]->FlushSourceBuffers();

		}
	}
}

void SoundEmitter::Pause()
{
	for (int i = 0; i < sourceVoice.size(); i++)
	{
		if (sourceVoice[i] != nullptr)
		{
			sourceVoice[i]->Stop();
		}
	}
}
void SoundEmitter::Resume()
{
	for (int i = 0; i < sourceVoice.size(); i++)
	{
		if (sourceVoice[i] != nullptr)
		{
			sourceVoice[i]->Start();
		}
	}
}

int SoundEmitter::SearchFreeSourceVoice()
{
	for (size_t i = 0; i < sourceVoice.size(); i++)
	{
		if (sourceVoice[i] == nullptr)
		{
			continue;
		}

		XAUDIO2_VOICE_STATE state{};
		sourceVoice[i]->GetState(&state);

		if (state.BuffersQueued == 0)
		{
			sourceVoice[i]->Stop();
			sourceVoice[i]->FlushSourceBuffers();
			return (int)i;
		}
	}
	return -1;
}
