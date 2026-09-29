#pragma once

//Defineクラス:プログラムで使用する定数を格納している
class Define final {
public:
	static const wchar_t APP_NAME[100];//このアプリケーション名
	static const float GameFPS;
	static const float GameTick;
	static const bool Debug;

	static const int WIN_W;	//ウィンドウサイズ横
	static const int WIN_H;	//ウィンドウサイズ縦

	static const float PI;	//円周率
	static const int ChipSize;

	static const float Gravity;
};
