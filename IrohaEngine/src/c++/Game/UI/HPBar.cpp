#include "HPBar.h"

void HPBar::Update()
{
	_position = targetEntity.GetPosition() + _offset;
}


void HPBar::Draw()const
{
	D3D.SetLayer(Layer_WorldUI);
	float size = 100;
	float thin = 10;
	float rate = _hp / _maxHP;
	
	D3D.DrawBox(_position.x - size / 2, _position.y - thin / 2, size, thin, D3D.GetColor(100, 100, 100));//土台
	D3D.DrawBox(_position.x - size / 2, _position.y - thin / 2, size - thin, thin - 3, D3D.GetColor(10, 10, 10));//中身
	D3D.DrawBox((_position.x - size / 2), _position.y - thin / 2, (size - thin)*rate, thin - 3, D3D.GetColor(60, 255, 100));//HP
	D3D.SetLayer(Layer_0);
}
