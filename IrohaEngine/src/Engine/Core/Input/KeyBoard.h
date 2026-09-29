#pragma once

#include "Core/Singleton.h"

//キーボードクラス：キーボード入力の更新を行うクラス。
class Keyboard final : public Singleton<Keyboard> 
{

public:
	Keyboard() = default;
	virtual ~Keyboard() = default;
	bool update();	//更新
	bool GetKeyDown(int keyCode);
	bool GetKey(int keyCode);
	bool GetKeyUp(int keyCode);

private:
	static const int KEY_NUM = 256;	//キー総数
	std::array<char, KEY_NUM>old_key;
	std::array<int, KEY_NUM>key;

	bool isAvailableCode(int keyCode);//keyCodeが有効なキー番号か問う
};
