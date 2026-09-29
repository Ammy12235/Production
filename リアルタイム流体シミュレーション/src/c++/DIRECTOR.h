#pragma once
#include "BASE.h"

//ディレクタークラス:デバイスの生成とメインループの保持をする
class DIRECTOR final: public CELEMENT
{
public:

	//Method
	DIRECTOR()=default;
	~DIRECTOR()=default;
	bool initialize(HINSTANCE hInstance)const;
	void finalize()const;
	void mainloop()const;
private:


};