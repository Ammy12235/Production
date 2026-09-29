#pragma once

#include "Core/Base.h"
#include "Core/DEFINE.h"
#include "Core/Component.h"

class IComponent;
constexpr UINT MAX_SoundNum = 64;//音を同時に出せる最大数
constexpr UINT MAX_SoundEmitterSoundNum = 10;//一つのエミッタから同時に音を出せる最大数

//サウンドエミッタークラス：設定されたサウンドを定位置から発生させるクラス。パンなどを設定すれば疑似的な3Dサウンドも可能。
class SoundEmitter :public IComponent
{
public:
	SoundEmitter();
	virtual ~SoundEmitter();
	void Update()override;
	void SetVolume(float volume);
	void SetPan(float pan);
	void Play(int id,bool loopFlag=0);
	void Stop();
	void Pause();
	void Resume();

private:
	bool _isPlaying = false;
	bool isFirstPlay = true;
	float _volume = 1.0f;
	float _pan = 0.0f;
	std::array<IXAudio2SourceVoice*, MAX_SoundEmitterSoundNum> sourceVoice;
	IXAudio2SourceVoice* sourceVoiceLoop = nullptr;
	DWORD dwChannelMask;
	int SearchFreeSourceVoice();//フリーなソースボイスを検索し、そのインデックスを返す。空きがなければ-1を返す。
};
