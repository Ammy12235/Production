#pragma once

#include "Core/Base.h"

//Xインプットクラス：XInputの設定をするクラス。XInputの振動機能、デッドゾーンなどを設定する。
class XInput
{
public:
	int CheckHitPad();
	bool SetPadVibration(int leftMotorSpeed, int rightMotorSpeed);
	void SetDeadZone(int magnitude);
	XMFLOAT2 GetAnalogStickInput()const;
private:
	XINPUT_VIBRATION vibration;
	XINPUT_STATE state;

	int deadZoneMagnitude;
	static inline XInput* s_instance;
	XInput(){}
	~XInput() {}
public:

	static void CreateInstance()
	{
		DeleteInstance();
		s_instance = new XInput();
	}

	static void DeleteInstance()
	{
		if (s_instance != nullptr)
		{
			delete s_instance;
			s_instance = nullptr;
		}
	}

	static XInput& GetInstance()
	{
		return *s_instance;
	}
protected:

};

#define XINPUT XInput::GetInstance()
