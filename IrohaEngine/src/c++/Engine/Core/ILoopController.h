#pragma once
#include "Singleton.h"

class ILoopController :public Singleton<ILoopController>
{
public:
	ILoopController()=default;
	virtual ~ILoopController()=default;

	void Pause(bool flag) { isPause = flag; }//ポーズ状態にする。（シーン内に書いた物は動かせるのでメニューやポーズ画面として用いることができる。）
	void Cleanup() { isCleanup = true; }
	void Exit() { isExit = true; }//ゲームループを終了させる。

	bool GetIsPause()//今ポーズ中かを返す
	{
		return isPause;
	}

	friend class EngineLoop;
private:

	void CleanupEnd() { isCleanup = false; }
	bool isPause = false;
	bool isExit=false;
	bool isCleanup = false;
};
