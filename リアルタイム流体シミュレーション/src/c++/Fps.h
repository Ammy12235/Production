#pragma once 

#include "Base.h"

class Fps:public CELEMENT
{
public:
	bool InitFps();
	void CalculationFps();
	void CalculationSleep();
	void CalculationFrameTime();
	double GetFps();
	double GetFrameTime();
	void DrawFps();
	void DrawFrameTime();

	LARGE_INTEGER m_freq = { 0 };
	LARGE_INTEGER m_starttime = { 0 };
	LARGE_INTEGER m_nowtime = { 0 };
	LARGE_INTEGER m_frametime_a = { 0 };
	LARGE_INTEGER m_frametime_b = { 0 };
	int m_iCount = 0;

	double g_dFps;
	double g_dFrameTime;

private:

	WCHAR fps[100];
	static inline Fps* s_instance;
	Fps() {}

public:
	static void CreateInstance()
	{
		DeleteInstance();
		s_instance = new Fps();
	}

	static void DeleteInstance()
	{
		if (s_instance != nullptr)
		{
			delete s_instance;
			s_instance = nullptr;
		}
	}

	static Fps& GetInstance()
	{
		return *s_instance;
	}

protected:
};

#define FPS Fps::GetInstance()