#pragma once
#include "Core/Base.h"
struct D3D_INIT
	{
		HWND hWnd;
	};
//
//
//
//ウインドウクラス：ウインドウの生成、初期化、削除をするクラス。
class WINDOW : public CELEMENT
{
public:
	//Data
	static HWND m_hWnd;

	

	//Method
	HRESULT InitWindow(HINSTANCE, INT, INT, INT, INT, LPWSTR);
	LRESULT MsgProc(HWND, UINT, WPARAM, LPARAM);
};
