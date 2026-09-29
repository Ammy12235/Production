#include "Cloudy.h"

Cloudy::Cloudy()
{
	_destHSV = XMFLOAT3(1, 0.7f, 1);//晴れと雨の間
	_weatherSound = AddComponent<SoundEmitter>();
	_weatherSound->SetVolume(0);

	weatherSoundId = SND_FAC.CreateSound("snd/se/ambient_wind_1.mp3");
}

void Cloudy::Update()
{
	//晴れの場合

	if (_state == WeatherState_In)//もし今回天気に選ばれてその天気になるなら
	{
		_timer += FPS.GetFrameSecondTime();

		if (_timer > _transTime)
		{
			_timer = _transTime;
		}
		float value = std::clamp(_timer / _transTime, 0.0f, 1.0f);//０～１で変化する値

		//HSVフィルターの彩度を線形補間しながら変化させる
		float saturation = _destHSV.y * value + startHSVValue.y * (1 - value);

		D3D.getHSVFilter()->setParameter(_destHSV.x, saturation, _destHSV.z);

		//風の音を徐々に大きくする
		_weatherSound->SetVolume(value);

		if (_timer == _transTime)
		{
			_state = WeatherState_Exist;
			_timer = 0.0f;
		}
	}
	else if (_state == WeatherState_Out)
	{
		_timer += FPS.GetFrameSecondTime();

		if (_timer > _transTime)
		{
			_timer = _transTime;
		}
		float value = std::clamp(1 - _timer / _transTime, 0.0f, 1.0f);//1～0で変化する値

		//風の音を徐々に小さくする
		_weatherSound->SetVolume(value);

		if (_timer == _transTime)
		{
			_state = WeatherState_NotExist;
			_timer = 0.0f;
		}
	}
	else if (_state == WeatherState_Exist)
	{

	}
	else if (_state == WeatherState_NotExist)
	{

	}


}
