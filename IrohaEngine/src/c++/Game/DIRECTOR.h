#pragma once

//ディレクタークラス:デバイスの生成とメインループの保持をするクラス。一番偉いクラス。
class DIRECTOR final
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
