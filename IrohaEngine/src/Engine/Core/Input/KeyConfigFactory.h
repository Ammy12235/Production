#pragma once
#include "Core/Base.h"
#include "Pad.h"

//キーコンフィグファクトリークラス：設定ファイルからキーコンフィグするための情報をロードするクラス。
class KeyConfigFactory
{
private:
	const char fileName[100] = "config/keyConfig.txt";
public:
	bool LoadKeyConfig(int* _idArray,size_t size);
	bool SetKeyConfig();
	static bool CreateFactory(KeyConfigFactory** kcFactory);
	KeyConfigFactory(){}
	~KeyConfigFactory(){}

};
