#include "Sunny.h"

Sunny::Sunny()
{
	_destHSV = XMFLOAT3(1, 1, 1);//デフォルトに戻す
	_weatherSound = AddComponent<SoundEmitter>();
	_weatherSound->SetVolume(0);

	weatherSoundId = SND_FAC.CreateSound("snd/se/ambient_wind_1.mp3");
	_godlayTex = TEX_FAC.CreateTexture("tex/background/godlay.png");
}

void Sunny::Update()
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

		//ゴッドレイを徐々に大きくする
		DrawgodLay(value);

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
		//ゴッドレイを徐々に小さくする
		DrawgodLay(value);
		if (_timer == _transTime)
		{
			_state = WeatherState_NotExist;
			_timer = 0.0f;
		}
	}
	else if (_state == WeatherState_Exist)
	{
		DrawgodLay(1);
	}
	else if (_state == WeatherState_NotExist)
	{

	}


}

void Sunny::DrawgodLay(float alpha)
{
	cameraPallaxPos = CAMERA.GetCameraPos();

	XMFLOAT2 diff = cameraPallaxPos - preCameraPallaxPos;
	godLayParallaxPos0 += diff * 0.8f;
	D3D.SetLayer(Layer_3);
	D3D.SetAlignmentBlendDesc(255 * alpha*0.5f);
	for (int i = 0; i < 10; i++)
	{
		D3D.DrawRotImage(_godlayTex, 1000 + i * 200+ godLayParallaxPos0.x + sin(234 * i)*40, 150+ godLayParallaxPos0.y, 70, 500+sin(23644*i)*200, 0.5f - i * 0.05f, 10);
	}
	D3D.SetDefaultBlendDesc(255);

	godLayParallaxPos1 += diff * 0.4f;
	D3D.SetLayer(Layer_2);
	D3D.SetAlignmentBlendDesc(255 * alpha * 0.5f);
	for (int i = 0; i < 10; i++)
	{
		D3D.DrawRotImage(_godlayTex, 1000 + i * 200 + godLayParallaxPos1.x + sin(23844 * i) * 40, 150 + godLayParallaxPos1.y, 70, 500 + sin(2365234 * i) * 200, 0.5f - i * 0.05f, 11);
	}
	D3D.SetDefaultBlendDesc(255);
	D3D.SetLayer(Layer_0);

	preCameraPallaxPos = cameraPallaxPos;
}
