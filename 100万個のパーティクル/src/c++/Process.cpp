#include "Process.h"
#include "DEFINE.h"
#include "DirectX.h"

void Process::SetStartTime()
{
	QueryPerformanceFrequency(&freq);
	QueryPerformanceCounter(&start);//処理にかかる時間の計測開始
};

void Process::SetEndTime()
{
	QueryPerformanceCounter(&end);//処理にかかった時間の計測終了
	time = static_cast<double>(end.QuadPart - start.QuadPart) * 1000.0 / freq.QuadPart;
};

double Process::GetProcess()
{
	return time;
};

void Process::DrawProcess()
{

		WCHAR timeStr[256];
		swprintf(timeStr, 256, L"Time:%2.3lf[ms]\n", time);//処理にかかった時間の表示
		if (time > (float)1 / 60 * 1000)
		{
			DWRITE.DrawFormatText(timeStr, (float)Define::WIN_W - 160, (float)Define::WIN_H - 100, 300, 30, D3D.GetColor(255, 255, 0), 1, 1);
		}
		else DWRITE.DrawFormatText(timeStr, (float)Define::WIN_W - 160, (float)Define::WIN_H - 100, 300, 30, D3D.GetColor(255, 255, 255), 1, 1);

};