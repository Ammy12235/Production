#include "Keyboard.h"
#include "DirectX.h"

bool Keyboard::update() {
	DINPUT.Re_Input();

	char nowKeyStatus[KEY_NUM];
	DINPUT.CheckHitKey(nowKeyStatus);       //今のキーの入力状態を取得

	for (int i = 0; i < KEY_NUM; i++) {
		key[i] = nowKeyStatus[i];
		if ((nowKeyStatus[i] & 0x80) && !(old_key[i] & 0x80))
		{
			key[i] = 2;
		}
		else if (!(nowKeyStatus[i] & 0x80) && (old_key[i] & 0x80))
		{
			key[i] = 3;
		}
		old_key[i] = nowKeyStatus[i];
	}
	return true;
}

bool Keyboard::GetKeyDown(int keyCode)
{
	if (!isAvailableCode(keyCode))return false;
	if (key[keyCode] == 2)
	{
		OutputDebugString(L"GetKeyDown\n");
		return true;
	}
	return false;

}

bool Keyboard::GetKey(int keyCode)
{
	if (!isAvailableCode(keyCode))return false;
	if (key[keyCode] & 0x80)
	{
		OutputDebugString(L"GetKey\n");
		return true;
	}

	return false;

}

bool Keyboard::GetKeyUp(int keyCode)
{
	if (!isAvailableCode(keyCode))return false;
	if (key[keyCode] == 3)
	{
		OutputDebugString(L"GetKeyUp\n");
		return true;
	}
	return false;

}

/*!
@brief keyCodeが有効な値か否かを返す
*/
bool Keyboard::isAvailableCode(int keyCode) {
	if (!(0 <= keyCode && keyCode < KEY_NUM)) {
		return false;
	}
	return true;
}