#pragma once
#include "BASE.h"

//
//
//
//ウインドウクラス：ウインドウの生成、初期化、管理、削除を行う。
class WINDOW : public CELEMENT
{
public:
	//Data
	static HWND m_hWnd;

	//Method
	HRESULT InitWindow(HINSTANCE, INT, INT, INT, INT, LPWSTR);
	LRESULT MsgProc(HWND, UINT, WPARAM, LPARAM);
};
