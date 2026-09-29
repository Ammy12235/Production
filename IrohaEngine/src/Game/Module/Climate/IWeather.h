#pragma once
#include <Iroha.h>

//天気を動的に切り替えるための機構のインターフェース
class IWeather :public Entity
{
public:
	IWeather() = default;
	virtual ~IWeather()=default;

	virtual void SetIn() {//天気を出す
		D3D.getHSVFilter()->setIsApply(true);
		startHSVValue = D3D.getHSVFilter()->getParameter();//色補正は共通なのでここで初期値を取得し、線形補完に備える
		_state = WeatherState_In;
	}
	;
	virtual void SetOut() {//天気を引っ込める
		_state = WeatherState_Out;
	}
	;

	void SetTransTime(float time)
	{
		_transTime = time;
	}



	virtual void Update()override {};

protected:
	enum eWeatherState
	{
		WeatherState_In,//発生させる
		WeatherState_Out,//消滅させる
		WeatherState_Exist,//維持する
		WeatherState_NotExist,//消えたまま
	};
	float _timer = 0.0f;
	float _transTime = 3.0f;

	XMFLOAT3 startHSVValue = {0,0,0};

	std::shared_ptr<SoundEmitter> _weatherSound;
	int weatherSoundId = 0;
	eWeatherState _state = WeatherState_NotExist;
};
