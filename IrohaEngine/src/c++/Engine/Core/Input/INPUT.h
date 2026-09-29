#pragma once
#include "Core/Singleton.h"

//インプットクラス：入力に使用するデバイスの初期化、更新、削除を行うクラス。
class Input:public Singleton<Input>
{
public:

	bool Initalize(HWND hWnd, HINSTANCE hInstance);
	void ReleaseInputDevice();
	Input()
	{

	}
	~Input()
	{
		ReleaseInputDevice();
	}
private:
	HWND hWnd = nullptr;

};

#define INPUT Input::GetInstance()
