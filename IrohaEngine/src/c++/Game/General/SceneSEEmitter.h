#pragma once
#include <Iroha.h>

class SceneSEEmitter : public Entity
{
public:
	SceneSEEmitter()
	{
		soundEmitter = AddComponent<SoundEmitter>();
	}
	;
	~SceneSEEmitter() = default;

	virtual void Update()override {};


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
			soundEmitter->Play(soundID);
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
