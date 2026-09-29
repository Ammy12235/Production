#pragma once

#include "Singleton.h"
#include <array>
#include "Base.h"

//マウスクラス：マウス入力の更新を行うクラス。
class Mouse final : public Singleton<Mouse> {

public:
	Mouse(){};
	~Mouse() = default;

public:
	bool update();	//更新

	bool GetMouseDown(int mouseButtonNum)const;
	bool GetMouse(int mouseButtonNum)const;
	bool GetMouseUp(int mouseButtonNum)const;
	void GetMouseCursorPos(POINT* point)const;
	void GetMouseVelocityX(float* vx)const;//マウスのX軸方向の運動量を取得
	void GetMouseVelocityY(float* vy)const;//マウスのY軸方向の運動量を取得
	void GetMouseVelocityZ(float* vz)const;//マウスホイールの運動量を取得
private:
	int old_mouse = 0;
	static const int MOUSE_BUTTON_NUM = 3;	//キー総数
	std::array<int, MOUSE_BUTTON_NUM> _mouse;//押されカウンタ
	bool isAvailableCode(int keyCode)const;//keyCodeが有効なキー番号か問う

};