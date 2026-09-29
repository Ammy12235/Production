#include "Mouse.h"
#include "DirectX.h"

void Mouse::GetMouseCursorPos(POINT* point)const
{
	GetCursorPos(point);
	// スクリーン座標をクライアント座標に変換する
	ScreenToClient(DINPUT.hWnd, point);	
}

void Mouse::GetMouseVelocityX(float* vx)const
{
	*vx=DINPUT.dim.lX;
}

void Mouse::GetMouseVelocityY(float* vy)const
{
	*vy = DINPUT.dim.lY;
}

void Mouse::GetMouseVelocityZ(float* vz)const
{
	*vz = DINPUT.dim.lZ;
}

/*!
@brief keyCodeのキーが押されているフレーム数を返す
*/
bool Mouse::update()
{
	int mouseInput = 0;
	mouseInput = DINPUT.CheckHitMouse();//マウスの入力状態を取得
	for (int i = 0; i < MOUSE_BUTTON_NUM; i++) {
		bool nowMouse = mouseInput & (1 << i);
		bool oldMouse = old_mouse & (1 << i);
		if ((nowMouse && !oldMouse)) {//前回押されておらず今回押された
			_mouse[i] = 2;
		}
		else if ((!nowMouse && oldMouse)) {//今回押されておらず前回押された
			_mouse[i] = 3;
		}
		else if (mouseInput & (1 << i))_mouse[i] = 1;//常に押されている
		else if (!(mouseInput & (1 << i)))_mouse[i] = 0;//常に離されている
	}
	old_mouse = mouseInput;
	return true;
}

bool Mouse::GetMouseDown(int mouseButtonNum)const
{
	if (!isAvailableCode(mouseButtonNum))return false;
	if (_mouse[mouseButtonNum] == 2)
	{
		OutputDebugString(L"GetMouseDown\n");
		return true;
	}
	return false;

}

bool Mouse::GetMouse(int mouseButtonNum)const
{
	if (!isAvailableCode(mouseButtonNum))return false;
	if (_mouse[mouseButtonNum] == 1)
	{
		OutputDebugString(L"GetMousePad\n");
		return true;
	}

	return false;

}

bool Mouse::GetMouseUp(int mouseButtonNum)const
{
	if (!isAvailableCode(mouseButtonNum))return false;
	if (_mouse[mouseButtonNum] == 3)
	{
		OutputDebugString(L"GetMouseUp\n");
		return true;
	}
	return false;

}

/*!
@brief keyCodeが有効な値か否かを返す
*/
bool Mouse::isAvailableCode(int mouseButtonNum)const {
	if (!(0 <= mouseButtonNum && mouseButtonNum < MOUSE_BUTTON_NUM)) {
		return false;
	}
	return true;
}