#pragma once
#include <Iroha.h>

class SceneBGMEmitter : public Entity
{
public:
	SceneBGMEmitter()
	{
		soundEmitter = AddComponent<SoundEmitter>();
	}
	;
	~SceneBGMEmitter() = default;

	virtual void Init()override;
	virtual void Update()override;


	void SetMaxVolume(float volume)
	{
		if (soundEmitter)
		{
			soundEmitter->SetVolume(volume);
		}
	}
	void Stop()
	{
		if (soundEmitter)
		{
			soundEmitter->Stop();
		}
	}

	void Play(int soundID)
	{
		if (soundEmitter)
		{
			soundEmitter->Play(soundID, true);
		}
	}

	void Pause()
	{
		if (soundEmitter)
		{
			soundEmitter->Pause();
		}
	}

	void Resume()
	{
		if (soundEmitter)
		{
			soundEmitter->Resume();
		}
	}
private:

	std::shared_ptr<SoundEmitter> soundEmitter = nullptr;
};
