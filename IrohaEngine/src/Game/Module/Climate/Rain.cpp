#include "Rain.h"


void Rain::Update()
{
	if (summonRate == 0)return;
	//一定時間たったら、特定の位置に雨を召喚する
	setRain(summonRate);

	for (int i = 0; i < _rains.size(); i++)
	{
		_rains[i].leftLife -= FPS.GetFrameSecondTime();
		if (_rains[i].leftLife < 0)
		{
			_rains[i].leftLife = 0;
		}
		_rains[i].pos.y += _rains[i].speed;
	}

}

void Rain::Draw()const
{
	if (summonRate == 0)return;
	D3D.SetAlignmentBlendDesc(100);
	for (int i = 0; i < _rains.size(); i++)
	{
		if (_rains[i].leftLife!=0)
		{
			D3D.DrawLine(_rains[i].pos.x, _rains[i].pos.y, _rains[i].pos.x, _rains[i].pos.y+100, D3D.GetColor(160,150, 160));

		}
	}
	D3D.SetDefaultBlendDesc(255);
}

void Rain::setRain(int summonRate)
{
	int count = 0;
	for (int i = 0; i < _rains.size(); i++)
	{
		if (_rains[i].leftLife == 0)//寿命が切れた雨があれば
		{
			_rains[i].leftLife = lifeTime;
			_rains[i].speed = rand() % 4 + 100;
			_rains[i].pos.x = _pos.x + rand() % 9000 - 4500;//
			_rains[i].pos.y = _pos.y;
			count++;
			if (count == summonRate)break;
		}
	}
}




