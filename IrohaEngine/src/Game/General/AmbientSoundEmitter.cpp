#include "AmbientSoundEmitter.h"

void AmbientSoundEmitter::Update()
{
	if (_isChangeSound)//もし、サウンドを変更するなら
	{
		_timer += FPS.GetFrameSecondTime();
		float timer = _timer / _changeTime;//０から１で変動させる
		float diff = -2 * timer * timer * timer + 3 * timer * timer;
		_soundEmitters[_index]->SetVolume(diff * _maxVolume);//設定した時間だけかけて遷移
		_soundEmitters[1 - _index]->SetVolume((1 - diff) * _maxVolume);//設定した時間だけかけて遷移
		if (_timer > _changeTime)
		{
			_soundEmitters[_index]->SetVolume(_maxVolume);//設定した時間だけかけて遷移
			_soundEmitters[1 - _index]->SetVolume(0);//設定した時間だけかけて遷移
			_soundEmitters[1 - _index]->Stop();//小さくした方をストップさせる
			_timer = 0.0f;
			_isChangeSound = false;//遷移終了
		}
	}
	
}

void AmbientSoundEmitter::SetMaxVolume(float volume)
{
	_maxVolume = volume;
}

void AmbientSoundEmitter::SetSoundTransition(int nextSoundId, float changeTime)
{
	if (_isChangeSound == false)//変更中でないなら変えられる
	{
		//セットする
		_index = 1 - _index;
		_isChangeSound = true;
		_changeTime = changeTime;
		_nextSoundId = nextSoundId;
		_soundEmitters[_index]->SetVolume(0);//ボリュームが０の状態で初めて、
		_soundEmitters[_index]->Play(_nextSoundId);//大きくなる方を再生し始める
	}

}

