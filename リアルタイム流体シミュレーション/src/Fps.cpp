#include "Fps.h"
#include "DEFINE.h"
#include "DirectX.h"

#define GameFPS 60
bool Fps::InitFps()
{
	Log("FPS計測の初期化開始...");
	g_dFps = 0; g_dFrameTime = 0;
	bool isqpf, isqpc = false;
	if (QueryPerformanceFrequency(&m_freq))isqpf = true;
	else isqpf = false;
	//現在の時間を取得（1フレーム目)
	if (QueryPerformanceCounter(&m_starttime))isqpc = true;
	else isqpc = false;

	Log("成功\n");

	if (!isqpf)
	{
		Log("失敗\n");
		Log("　　　パフォーマンス カウンターの頻度を取得できませんでした\n");
		return false;
	}
	if (!isqpc)
	{
		Log("失敗\n");
		Log("　　　パフォーマンス カウンターの値を取得できませんでした\n");
		return false;
	}
	return true;
}


void Fps::CalculationFps()
{
	if (m_iCount == GameFPS)//カウントが60の時の処理
	{
		QueryPerformanceCounter(&m_nowtime);//現在の時間を取得（60フレーム目）
		//FPS = 1秒 / 1フレームの描画にかかる時間
		//    = 1000ms / ((現在の時間ms - 1フレーム目の時間ms) / 60フレーム)
		g_dFps = 1000.0 / (static_cast<double>((m_nowtime.QuadPart - m_starttime.QuadPart) * 1000 / m_freq.QuadPart) / GameFPS);
		m_iCount = 0;//カウントを初期値に戻す
		m_starttime = m_nowtime;//1フレーム目の時間を現在の時間にする
	}
	m_iCount++;//カウント+1
}

void Fps::CalculationSleep()
{
	QueryPerformanceCounter(&m_nowtime);//現在の時間を取得
	 //Sleepさせる時間ms = 1フレーム目から現在のフレームまでの描画にかかるべき時間ms - 1フレーム目から現在のフレームまで実際にかかった時間ms
	 //                  = (1000ms / 60)*フレーム数 - (現在の時間ms - 1フレーム目の時間ms)
	DWORD dwSleepTime = static_cast<DWORD>((1000.0 / GameFPS) * m_iCount - (m_nowtime.QuadPart - m_starttime.QuadPart) * 1000 / m_freq.QuadPart);
	if (dwSleepTime > 0 && dwSleepTime < 18)//大きく変動がなければSleepTimeは1～17の間に納まる
	{
		timeBeginPeriod(1);
		Sleep(dwSleepTime);
		timeEndPeriod(1);
	}
	else//大きく変動があった場合
	{
		timeBeginPeriod(1);
		Sleep(1);
		timeEndPeriod(1);
	}
}

void Fps::CalculationFrameTime()
{
	static int iFlg;
	if (iFlg == 0)
	{
		QueryPerformanceCounter(&m_frametime_a);
		iFlg = 1;
	}
	QueryPerformanceCounter(&m_frametime_b);
	g_dFrameTime = (m_frametime_b.QuadPart - m_frametime_a.QuadPart) * 1000.0 / m_freq.QuadPart;
	m_frametime_a = m_frametime_b;
}

double Fps::GetFps()
{
	return g_dFps;
}

//--------------------------------------------------------------------------------------
// Window::GetFrameTime()関数：1フレームあたりの時間の取得
//--------------------------------------------------------------------------------------
double Fps::GetFrameTime()
{
	return g_dFrameTime;
}

void Fps::DrawFps()
{
	//FPSの表示
	swprintf(fps, 256, L"FPS=%3.2lf", FPS.GetFps());
	DWRITE.DrawFormatText(fps, Define::WIN_W - 260, Define::WIN_H - 70, 160, 30, D3D.GetColor(255, 255, 255), 1, 1);
}

void Fps::DrawFrameTime()
{
	WCHAR ft[256];
	//FTの表示
	swprintf(ft, 256, L"FlameTime=%3.2lf", FPS.GetFrameTime());
	DWRITE.DrawFormatText(ft, Define::WIN_W - 260, Define::WIN_H - 90, 160, 30, D3D.GetColor(255, 255, 255), 1, 1);
}