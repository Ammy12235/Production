#pragma once
#include "Core/Singleton.h"

static constexpr float MaxShakeOffset=50;
enum ShakeType
{
	Shake_Random,//ランダムな方向に揺れ、徐々に収束
	Shake_XSin,//X方向にサイン波の揺れ、徐々に収束
	Shake_XCos,//X方向にサイン波の揺れ、徐々に収束
	Shake_YSin,//X方向にサイン波の揺れ、徐々に収束
	Shake_YCos,//X方向にサイン波の揺れ、徐々に収束
	Shake_XSinRot,//X方向にサイン波の揺れ、徐々に収束、回転もする。
	Shake_XCosRot,//X方向にサイン波の揺れ、徐々に収束、回転もする。
	Shake_YSinRot,//X方向にサイン波の揺れ、徐々に収束
	Shake_YCosRot,//X方向にサイン波の揺れ、徐々に収束
	Shake_Spiral,//回りながら収束
	Shake_SpiralRot,//回りながら、回転もする。
	Shake_ScaleInpulsePush,//スケールのインパルス、すぐに収束。
};
struct ResultShakeParameter
{
	XMFLOAT2 resultPositionOffset = { 0,0 };//位置のオフセット
	float resultRotationOffset = 0;//回転のオフセット
	float resultScaleOffset = 0;              //スケールのオフセット
};

class CameraShaker :public Singleton<CameraShaker>
{
private:

	struct ShakeParameter
	{
		bool isActive = false;
		float time = 0.0f;
		ShakeType shakeType = Shake_Random;
		XMFLOAT2 shakeOffset = { 0,0 };
		float magnitude = 0.0f;
	};


public:
	CameraShaker() = default;
	~CameraShaker() = default;
	void GenerateShake(ShakeType type, float duration, float magnitude);//カメラ振動を登録、１０個まで同時に登録可能。
	void Update();//カメラの更新関数で更新される

	ResultShakeParameter GetShakeOffset()const { return resultShakeParameter; }//振動結果を返す。

private:

	float shakeTime = 1.5f;


	ResultShakeParameter resultShakeParameter;
	std::array<ShakeParameter, 10> shakeParameters;
};
