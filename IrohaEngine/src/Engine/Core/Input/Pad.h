#pragma once

#include "Core/Singleton.h"

enum ePad {
	left,
	up,
	right,
	down,
	jump,
	coop,
	indivSub,
	indivMain,
	finalSub,
	finalMain,
	pause,
	max
};

//パッドクラス：パッド入力の更新を行う。
class Pad final : public Singleton<Pad> {

private:
	void merge();
	const static int PAD_KEY_NUM = 16;
	std::array<int, PAD_KEY_NUM> pad;      //16ボタンのpad入力状態格納
	int old_pad = 0;

public:
	Pad();
	~Pad() = default;
	int _idArray[PAD_KEY_NUM];  //どのボタンがどのボタンに割り当たっているかを示す
	void update();

	bool GetPadDown(ePad eID)const;
	bool GetPad(ePad eID)const;
	bool GetPadUp(ePad eID)const;
	XMFLOAT2 GetAnalogStickInput()const;
	bool SetPadVibration(int leftMotorSpeed, int rightMotorSpeed);
	bool isAvailableCode(ePad eID)const;
public:

};
