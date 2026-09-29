#pragma once 

#include "Core/Singleton.h"

//FPSクラス。ゲームのフレームレートを計算、表示するクラス。
class Fps:public Singleton<Fps>
{
public:
	bool InitFps();
	void CalculationFps();
	void CalculationSleep();
	void CalculationFrameTime();
	double GetFps();
	double GetFrameMiliTime();
	double GetFrameSecondTime();
	void DrawFps();

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

public:

	Fps()
	{
		InitFps();
	}
	virtual ~Fps(){}

protected:
};

#define FPS Fps::GetInstance()
