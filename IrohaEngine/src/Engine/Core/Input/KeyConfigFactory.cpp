#include "KeyConfigFactory.h"

bool KeyConfigFactory::CreateFactory(KeyConfigFactory** ppKcFactory)
{
	KeyConfigFactory* pKcFactory = new KeyConfigFactory();
	*ppKcFactory = pKcFactory;
	return true;
}
bool KeyConfigFactory::LoadKeyConfig(int* _idArray,size_t size)
{
	FILE* fp;
	fopen_s(&fp, fileName, "r");
	if (!fp)
	{
		MSG(L"キー設定ファイルを読み込めませんでした。\n初期設定を適用します");
		for(size_t i=0;i<size;i++)
		{
			_idArray[i] = -1;
		}
		_idArray[ePad::left]      = 0;
		_idArray[ePad::up]        = 1;
		_idArray[ePad::right]     = 2;
		_idArray[ePad::down]      = 3;
		_idArray[ePad::jump]      = 6;
		_idArray[ePad::coop]      = 7;
		_idArray[ePad::indivMain] = 9;
		_idArray[ePad::indivSub]  = 8;
		_idArray[ePad::finalMain] = 15;
		_idArray[ePad::finalSub]  = 14;
		_idArray[ePad::pause]     = 11;

		return false;
	}


	for (size_t i = 0; i < size; i++)
	{
		_idArray[i] = -1;
	}
	int i = 0, num = 0;
	int input[8];
	char inputc[8];
	while (1) {
		for (i = 0; i < 64; i++) {//意味ある塊として認識できるようになったら
			inputc[i] = input[i] = fgetc(fp);//1文字取得する
			if (inputc[i] == '/') {//スラッシュがあれば
				while (fgetc(fp) != '\n');//改行までループ
				i = -1;//カウンタを最初に戻して
				continue;
			}
			if (input[i] == ',' || input[i] == '\n') {//カンマか改行なら
				inputc[i] = '\0';//そこまでを文字列とし

				break;
			}
			if (input[i] == EOF) {//ファイルの終わりなら
				goto EXFILE;//終了
			}
		}
		
		switch (num)//16個のボタンに入れ込んでいく
		{
		case 0:_idArray[ePad::left] = atoi(inputc); break;
		case 1:_idArray[ePad::up] = atoi(inputc); break;
		case 2:_idArray[ePad::right] = atoi(inputc); break;
		case 3:_idArray[ePad::down] = atoi(inputc); break;
		case 4:_idArray[ePad::jump] = atoi(inputc); break;
		case 5:_idArray[ePad::indivMain] = atoi(inputc); break;
		case 6:_idArray[ePad::indivSub] = atoi(inputc); break;
		case 7:_idArray[ePad::coop] = atoi(inputc); break;
		case 8:_idArray[ePad::finalMain] = atoi(inputc); break;
		case 9:_idArray[ePad::finalSub] = atoi(inputc); break;
		case 10:_idArray[ePad::pause] = atoi(inputc); break;
		
		}
		num++;
		if (num == size)goto EXFILE;

	}
EXFILE:
	fclose(fp);
	return true;
}

bool  KeyConfigFactory::SetKeyConfig()
{
	return false;
}
