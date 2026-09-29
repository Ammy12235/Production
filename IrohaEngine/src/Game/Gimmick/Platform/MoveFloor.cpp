#include "MoveFloor.h"

MoveFloor::MoveFloor(IdInfo idInfo)
{
	_idInfo._tag = "Gimmick";
	_collider = AddComponent<RectangleCollider>();
	//_collider.get()->setRect()
}

void MoveFloor::Init()
{

}

void MoveFloor::Update()
{
	s++;
	
	_v.x = 3*sin(s / 80);
	
	SetVelocity(_v);
	
}

void MoveFloor::Draw()const
{
	D3D.DrawBox(_position.x, _position.y, 64, 14, D3D.GetColor(255, 255, 0));
}
