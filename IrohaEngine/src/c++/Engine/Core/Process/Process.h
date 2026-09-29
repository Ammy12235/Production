#pragma once

#include "Core/Singleton.h"

//プロセスクラス：処理の時間に関するクラス。処理にかかる時間を計測する。
class Process :public Singleton<Process>
{
public:
	void SetStartTime();
	void SetEndTime();
	double GetProcess();

	void DrawProcess();

	Process() {}
	~Process() {}
private:

	LARGE_INTEGER freq;
	LARGE_INTEGER start, end;

	double time;


};

#define PROCESS Process::GetInstance()
