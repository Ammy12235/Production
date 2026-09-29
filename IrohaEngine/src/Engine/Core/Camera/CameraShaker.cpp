#include "CameraShaker.h"
#include "Core/Process/Fps.h"


void CameraShaker::GenerateShake(ShakeType type, float duration, float magnitude)//カメラ振動を登録、１０個まで同時に登録可能。
{
	ShakeParameter param;
	param.time = duration;
	param.shakeType = type;
	param.magnitude = magnitude;

	for (int i = 0; i < shakeParameters.size(); i++)
	{
		if (shakeParameters[i].isActive == false)
		{
			shakeParameters[i] = param;
			shakeParameters[i].isActive = true;//有効にする
			break;
		}
	}
}
void CameraShaker::Update()//カメラの更新関数で更新される
{
	XMFLOAT2 totalPosition = { 0,0 };
	float totalRotation = 0;
	float totalScale = 0;//初期値は0.01fとする。
	UINT count = 0;
	for (int i = 0; i < shakeParameters.size(); i++)
	{
		if (shakeParameters[i].isActive == false)continue;//有効化されていないスロットなら計算しない。

		switch (shakeParameters[i].shakeType)//振動のタイプによる分岐
		{
		case ShakeType::Shake_Random:
			totalPosition.x += (float)(rand() % 100 - 50) / 50;//-1から1までの乱数
			totalPosition.y += (float)(rand() % 100 - 50) / 50;
			break;
		case ShakeType::Shake_XSin:
			totalPosition.x += sin(shakeParameters[i].time * 10);//-1から1
			break;
		case ShakeType::Shake_XCos:
			totalPosition.x += cos(shakeParameters[i].time * 10);//-1から1
			break;
		case ShakeType::Shake_XSinRot:
			totalPosition.x += sin(shakeParameters[i].time * 10);//-1から1
			break;
		case ShakeType::Shake_XCosRot:
			totalPosition.x += sin(shakeParameters[i].time * 10);//-1から1
			break;
		case ShakeType::Shake_Spiral:
			totalPosition.x += sin(shakeParameters[i].time * 10);//-1から1
			break;
		case ShakeType::Shake_SpiralRot:
			totalPosition.x += cos(shakeParameters[i].time * 10);//-1から1
			break;
		case ShakeType::Shake_ScaleInpulsePush:
			totalScale += sin(shakeParameters[i].time * 10);//0から1
			break;
		default:
			break;

		}
		totalPosition *= shakeParameters[i].magnitude;
		shakeParameters[i].time -= FPS.GetFrameSecondTime();//残り時間を減らす。
		if (shakeParameters[i].time < 0)//もし残り時間が0になったら、
		{
			shakeParameters[i].isActive = false;//無効化する
			shakeParameters[i].time = 0.0f;
			shakeParameters[i].shakeType = Shake_Random;
			shakeParameters[i].shakeOffset = { 0,0 };
			shakeParameters[i].magnitude = 0.0f;
		}
		count++;
	}
	if (count > 0)//一つでも有効な振動があったら、
	{
		totalPosition /= count;//位置はすべての点を加算した平均点とする

	}

	if (totalPosition.x > MaxShakeOffset)totalPosition.x = MaxShakeOffset;
	if (totalPosition.x < -MaxShakeOffset)totalPosition.x = -MaxShakeOffset;
	if (totalPosition.y > MaxShakeOffset)totalPosition.y = MaxShakeOffset;
	if (totalPosition.y < -MaxShakeOffset)totalPosition.y = -MaxShakeOffset;

	resultShakeParameter.resultPositionOffset = totalPosition;
	resultShakeParameter.resultRotationOffset = totalRotation;
	resultShakeParameter.resultScaleOffset = totalScale;
}
