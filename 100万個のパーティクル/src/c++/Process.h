#pragma once
#include "Base.h"

//プロセスクラス：処理の時間に関するクラス。処理にかかる時間を計測する。
class Process :public CELEMENT
{
public:
	void SetStartTime();
	void SetEndTime();
	double GetProcess();

	void DrawProcess();

private:

	LARGE_INTEGER freq;
	LARGE_INTEGER start, end;

	double time;

	static inline Process* s_instance;
	Process() {}
	~Process() {}

public:
	static void CreateInstance()
	{
		DeleteInstance();
		s_instance = new Process();
	}

	static void DeleteInstance()
	{
		if (s_instance != nullptr)
		{
			delete s_instance;
			s_instance = nullptr;
		}
	}

	static Process& GetInstance()
	{
		return *s_instance;
	}

protected:
};

#define PROCESS Process::GetInstance()