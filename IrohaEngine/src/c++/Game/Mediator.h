#pragma once
#include "Parts.h"

class Parts;

//使い道はあまりない
//Mediatorクラス：Mediatorのインターフェースクラス。
class Mediator
{
protected:
	// コンストラクタをprotected宣言（生成の禁止）
	virtual void CreateParts() = 0;    // パーツを生成
	virtual void DeleteParts() = 0;    // パーツを削除
public:

	virtual void update()=0;    // 画面更新

	// 変化したパーツに対する振る舞いを定義(オブジェクトのポインターを渡して内部で処理する)
	virtual void PartsChanged(Parts* parts) = 0;
};
