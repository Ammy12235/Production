#include "Core/Define.h"
#include "Pad.h"
#include "KeyBoard.h"
#include "KeyConfigFactory.h"
#include "DINPUT.h"
#include "XINPUT.h"

using namespace std;

Pad::Pad()
{
	KeyConfigFactory* kcFactory;
	KeyConfigFactory::CreateFactory(&kcFactory);
	kcFactory->LoadKeyConfig(_idArray, PAD_KEY_NUM);

	SAFE_DELETE(kcFactory);
}

void Pad::update()
{
	int padInput = 0;
	padInput = max(DINPUT.CheckHitPad(), XINPUT.CheckHitPad());//パッドの入力状態を取得
	for (int i = 0; i < PAD_KEY_NUM; i++) {
		bool nowPad = padInput & (1 << i);
		bool oldPad = old_pad & (1 << i);
		if ((nowPad && !oldPad)) {//前回押されておらず今回押された
			pad[i] = 2;
		}
		else if ((!nowPad && oldPad)) {//今回押されておらず前回押された
			pad[i] = 3;
		}
		else if (padInput & (1 << i))pad[i] = 1;//常に押されている
		else if (!(padInput & (1 << i)))pad[i] = 0;//常に離されている
	}
	old_pad = padInput;
	merge();
}

bool Pad::GetPadDown(ePad eID)const
{
	if (!isAvailableCode(eID))return false;
	if (_idArray[eID] == -1)return false;
	if (pad[_idArray[eID]] == 2)
	{
		OutputDebugString(L"GetPadDown\n");
		return true;
	}
	return false;

}

bool Pad::GetPad(ePad eID)const
{
	if (!isAvailableCode(eID))return false;
	if (_idArray[eID] == -1)return false;
	if (pad[_idArray[eID]] == 1)
	{
		OutputDebugString(L"GetPad\n");
		return true;
	}

	return false;

}

bool Pad::GetPadUp(ePad eID)const
{
	if (!isAvailableCode(eID))return false;
	if (_idArray[eID] == -1)return false;
	if (pad[_idArray[eID]] == 3)
	{
		OutputDebugString(L"GetPadUp\n");
		return true;
	}
	return false;

}

XMFLOAT2 Pad::GetAnalogStickInput()const
{
	return XINPUT.GetAnalogStickInput();
}
bool Pad::SetPadVibration(int leftMotorSpeed, int rightMotorSpeed)
{
	return XINPUT.SetPadVibration(leftMotorSpeed, rightMotorSpeed);
}
/*!
@brief Padが有効な値か否かを返す
*/
bool Pad::isAvailableCode(ePad eID)const {
	if ((0 <= _idArray[eID] && _idArray[eID] < PAD_KEY_NUM)==false) {
		OutputDebugString(L"None");
		return false;
	}
	return true;
}
/*!
@brief パッドと、それに対応するキーボードの入力状態をマージする
*/
void Pad::merge()
{
/*
	pad[_idArray[ePad::left]] = max(pad[_idArray[ePad::left]], Keyboard::GetInstance()->GetKeyDown(DIK_LEFTARROW));
	pad[_idArray[ePad::up]] = max(pad[_idArray[ePad::up]], Keyboard::GetInstance()->getPressingCount(DIK_UPARROW));
	pad[_idArray[ePad::right]] = max(pad[_idArray[ePad::right]], Keyboard::GetInstance()->getPressingCount(DIK_RIGHTARROW));
	pad[_idArray[ePad::down]] = max(pad[_idArray[ePad::down]], Keyboard::GetInstance()->getPressingCount(DIK_DOWNARROW));
	pad[_idArray[ePad::jump]] = max(pad[_idArray[ePad::jump]], Keyboard::GetInstance()->getPressingCount(DIK_SPACE));
	pad[_idArray[ePad::coop]] = max(pad[_idArray[ePad::coop]], Keyboard::GetInstance()->getPressingCount(DIK_NUMPADENTER));
	pad[_idArray[ePad::indivMain]] = max(pad[_idArray[ePad::indivMain]], Keyboard::GetInstance()->getPressingCount(DIK_X));
	pad[_idArray[ePad::indivSub]] = max(pad[_idArray[ePad::indivSub]], Keyboard::GetInstance()->getPressingCount(DIK_LSHIFT));
	pad[_idArray[ePad::finalMain]] = max(pad[_idArray[ePad::finalMain]], Keyboard::GetInstance()->getPressingCount(DIK_ESCAPE));
	pad[_idArray[ePad::finalSub]] = max(pad[_idArray[ePad::finalSub]], Keyboard::GetInstance()->getPressingCount(DIK_LCONTROL));
	pad[_idArray[ePad::pause]] = max(pad[_idArray[ePad::pause]], Keyboard::GetInstance()->getPressingCount(DIK_C));
*/
	
}
