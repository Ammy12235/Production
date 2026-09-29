#include "XInput.h"

int XInput::CheckHitPad()
{
	DWORD dwResult;
	//ゲームパッドの状態を取得
	dwResult=XInputGetState(0, &state);

	if (dwResult == ERROR_SUCCESS)
	{
		unsigned int flag = 0;
		if (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT)flag |= (1 << 0);//ゲームパッド十字キー左
		if (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP)flag |= (1 << 1);//ゲームパッド十字キー上
		if (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT)flag |= (1 << 2);//ゲームパッド十字キー右
		if (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN)flag |= (1 << 3);//ゲームパッド十字キー下

		if (state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER)flag |= (1 << 4);//ゲームパッドLB
		if (state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) flag |= (1 << 5);//ゲームパッドRB

		if (state.Gamepad.wButtons & XINPUT_GAMEPAD_A) flag |= (1 << 6);//ゲームパッドA
		if (state.Gamepad.wButtons & XINPUT_GAMEPAD_B) flag |= (1 << 7);//ゲームパッドB
		if (state.Gamepad.wButtons & XINPUT_GAMEPAD_X) flag |= (1 << 8);//ゲームパッドX
		if (state.Gamepad.wButtons & XINPUT_GAMEPAD_Y) flag |= (1 << 9);//ゲームパッドY

		if (state.Gamepad.wButtons & XINPUT_GAMEPAD_START) flag |= (1 << 11);//ゲームパッドSTART
		if (state.Gamepad.wButtons & XINPUT_GAMEPAD_BACK) flag |= (1 << 10);//ゲームパッドBACK
		if (state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB) flag |= (1 << 12);//ゲームパッド左押し込み
		if (state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB) flag |= (1 << 13);//ゲームパッド右押し込み

		if (state.Gamepad.bLeftTrigger) flag |= (1 << 14);//ゲームパッドLT
		if (state.Gamepad.bRightTrigger) flag |= (1 << 15);//ゲームパッドRT

		return flag;
	}
	else return 0;
}

XMFLOAT2 XInput::GetAnalogStickInput()const
{
	//ゲームパッドアナログスティックのデッドゾーン処理
	if ((state.Gamepad.sThumbLX < XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE && state.Gamepad.sThumbLX > -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) &&
		(state.Gamepad.sThumbLY < XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE && state.Gamepad.sThumbLY > -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE))
	{
		return XMFLOAT2(0, 0);
	}

	return XMFLOAT2(state.Gamepad.sThumbLX, state.Gamepad.sThumbLY);
}

bool XInput::SetPadVibration(int leftMotorSpeed, int rightMotorSpeed)
{
	DWORD dwResult;
	vibration.wLeftMotorSpeed = leftMotorSpeed;
	vibration.wRightMotorSpeed = rightMotorSpeed;
	dwResult = XInputSetState(0, &vibration);
	if (dwResult == ERROR_SUCCESS)return false;

	return true;
}

void XInput::SetDeadZone(int magnitude)
{
	deadZoneMagnitude = magnitude;
}
