#pragma once
#include <Iroha.h>

class InstantSEEmitter :public Entity
{

public:
	InstantSEEmitter(int soundId, float lifeTime) :_lifeTime(lifeTime)
	{
		_soundEmitter = AddComponent<SoundEmitter>();
		if (_soundEmitter)
		{
			_soundEmitter->Play(soundId);
		}
	}

	void Update()override
	{
		_timer += FPS.GetFrameSecondTime();
		if (_timer > _lifeTime)
		{
			_soundEmitter->Stop();
			Destroy(*this);
		}

	}
	virtual ~InstantSEEmitter()
	{

	}
private:
	std::shared_ptr<SoundEmitter> _soundEmitter = nullptr;
	float _timer = 0.0f;
	float _lifeTime = 0.0f;
};
