#include "Rainy.h"


Rainy::Rainy()
{
	_destHSV = XMFLOAT3(1, 0.5, 1);//彩度を落とす
	_rainParticle = Instantiate<Rain>(XMFLOAT2(800, 0));
	_rainParticle->SetRainRate(0);
	_weatherSound = AddComponent<SoundEmitter>();
	_weatherSound->SetVolume(0);

	weatherSoundId = SND_FAC.CreateSound("snd/se/ambient_rain_1.mp3");
}


void Rainy::SetIn()
{
	__super::SetIn();

	//大気表現モジュールを生成
	for (int i = 0; i < 7; i++)
	{
		auto module = Instantiate<UpAdvection>();
		int randomValue = rand() % 1600 - 800;
		module->SetPosition(XMFLOAT2(800 + randomValue, -500));//横にランダムに並べる

		randomValue = rand() % 4 - 2;
		module->SetVelocity(XMFLOAT2(randomValue, 20));
	}

	_weatherSound->Play(weatherSoundId);
	D3D.getDropletFilter()->setIsApply(true);

}

void Rainy::Update()
{
	//雨の場合

	dynamicTimer += 0.01f;
	staticTimer += 0.03f;
	if (dynamicTimer > 0.01f)
	{
		isAddDynamic = true;
		dynamicTimer = 0;
	}
	if (staticTimer > 0.01f)
	{
		isAddStatic = true;
		staticTimer = 0.0f;
	}


	if (_state== WeatherState_In)//もし今回天気に選ばれてその天気になるなら
	{
		_timer += FPS.GetFrameSecondTime();

		if (_timer > _transTime)
		{
			_timer = _transTime;
		}
		float value = std::clamp(_timer / _transTime,0.0f,1.0f);//０～１で変化する値

		//HSVフィルターの彩度を線形補間しながら変化させる
		float saturation = _destHSV.y * value + startHSVValue.y * (1 - value);

		D3D.getHSVFilter()->setParameter(_destHSV.x, saturation, _destHSV.z);

		//水滴シェーダの歪みを徐々に大きくする
		float distortion = destDistortion * value + startDistortion * (1 - value);
		// 値のセット
		D3D.getDropletFilter()->setParameter(distortion, 0.02f,
			XMFLOAT2((float)(rand() % 100) * 0.01f, (float)(rand() % 100) * 0.01f),
			XMFLOAT2((float)(rand() % 100) * 0.01f, (float)(rand() % 100) * 0.01f), isAddDynamic, isAddStatic);

		//雨のパーティクルの発生レートを徐々に大きくする
		int rainRate = (int)(destRainRate * value + startRainRate * (1 - value));
		_rainParticle->SetRainRate(rainRate);

		//雨の音を徐々に大きくする
		_weatherSound->SetVolume(value);

		if (_timer == _transTime)
		{
			_state = WeatherState_Exist;
			_timer = 0.0f;
		}
	}
	else if (_state==WeatherState_Out)
	{
		_timer += FPS.GetFrameSecondTime();

		if (_timer > _transTime)
		{
			_timer = _transTime;
		}
		float value = std::clamp(1-_timer / _transTime,0.0f,1.0f);//1～0で変化する値

		//水滴シェーダの歪みを徐々に小さくする
		float distortion = destDistortion * value + startDistortion * (1 - value);
		// 値のセット
		D3D.getDropletFilter()->setParameter(distortion, 0.02f,
			XMFLOAT2((float)(rand() % 100) * 0.01f, (float)(rand() % 100) * 0.01f),
			XMFLOAT2((float)(rand() % 100) * 0.01f, (float)(rand() % 100) * 0.01f), isAddDynamic, isAddStatic);

		//雨のパーティクルの発生レートを徐々に小さくする
		int rainRate = (int)(destRainRate * value + startRainRate * (1 - value));
		_rainParticle->SetRainRate(rainRate);

		//雨の音を徐々に小さくする
		_weatherSound->SetVolume(value);

		if (_timer == _transTime)
		{
			D3D.getDropletFilter()->setIsApply(false);
			_state = WeatherState_NotExist;
			_timer = 0.0f;
		}
	}
	else if(_state == WeatherState_Exist)
	{
		D3D.getDropletFilter()->setParameter(destDistortion, 0.02f,
			XMFLOAT2((float)(rand() % 100) * 0.01f, (float)(rand() % 100) * 0.01f),
			XMFLOAT2((float)(rand() % 100) * 0.01f, (float)(rand() % 100) * 0.01f), isAddDynamic, isAddStatic);
	}
	else if (_state == WeatherState_NotExist)
	{

	}

	isAddDynamic = false;
	isAddStatic = false;
}
