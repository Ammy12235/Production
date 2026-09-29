#pragma once
#include <Iroha.h>

class AmbientSoundEmitter :public Entity
{
public:
	AmbientSoundEmitter(int soundId)
	{
		_currentSoundId = soundId;
		for (int i = 0; i < _soundEmitters.size(); i++)
		{
			_soundEmitters[i] = AddComponent<SoundEmitter>();
		}
		if (_soundEmitters[0] != nullptr)//もし最初のエミッターが生成されていたら、
		{
			_soundEmitters[0]->Play(soundId);//設定したサウンドを再生し始める
		}
	};
	virtual ~AmbientSoundEmitter() = default;

	void SetMaxVolume(float volume);
	void Stop()
	{
		for (int i=0;i<_soundEmitters.size();i++)
		{
			if (_soundEmitters[i] != nullptr)
			{
				_soundEmitters[i]->Stop();
			}
		}
	}
	void SetSoundTransition(int nextSoundId, float changeTime);

	void Update()override;
private:
	std::array<std::shared_ptr<SoundEmitter>, 2> _soundEmitters;

	int _nextSoundId = 0;//次再生する音
	int _currentSoundId = 0;//今の再生している音
	bool _isChangeSound = false;
	float _changeTime = 0.0;
	float _timer = 0.0f;

	int _index = 0;

	float _maxVolume = 1;
};
