#include "INPUT.h"
#include "DINPUT.h"
#include "XINPUT.h"


bool Input::Initalize(HWND hWnd, HINSTANCE hInstance)
{
	DirectInput::CreateInstance();

	if (!DINPUT.Initialize(hWnd, hInstance))
	{
		return false;
	}
	XInput::CreateInstance();

	Input::hWnd = hWnd;
	return true;
}


void Input::ReleaseInputDevice()
{
	DirectInput::DeleteInstance();
	XInput::DeleteInstance();
}

