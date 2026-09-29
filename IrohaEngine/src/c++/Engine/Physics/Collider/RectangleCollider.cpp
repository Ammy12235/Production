#include "RectangleCollider.h"
#include "CollisionTest.h"
#include "Core/DEFINE.h"
#include "Core/Entity.h"
#include <IrohaGraphics.h>

RectangleCollider::RectangleCollider()
{
	
	setRect(_pos, _size);
}

void RectangleCollider::Update()
{
	if (_isDelete)return;//削除フラグが立っていたら更新しない
	//位置の更新
	_pos = _entity->GetPosition() + _offset;
	setRect(_pos, _size);//矩形が更新される

	_aabb.x0 = _rect.Left;
	_aabb.y0 = _rect.Top;
	_aabb.x1 = _rect.Right;
	_aabb.y1 = _rect.Bottom;
}
bool RectangleCollider::draw()const
{
	if (enabled)D3D.SetAlignmentBlendDesc(200);
	else D3D.SetAlignmentBlendDesc(10);
	D3D.DrawLine(_rect.Right, _rect.Top, _rect.Left, _rect.Top, D3D.GetColor(0, 255, 0));
	D3D.DrawLine(_rect.Right, _rect.Top, _rect.Right, _rect.Bottom, D3D.GetColor(0, 255, 0));
	D3D.DrawLine(_rect.Right, _rect.Bottom, _rect.Left, _rect.Bottom, D3D.GetColor(0, 255, 0));
	D3D.DrawLine(_rect.Left, _rect.Bottom, _rect.Left, _rect.Top, D3D.GetColor(0, 255, 0));
	D3D.SetDefaultBlendDesc(255);
	return true;
}

bool RectangleCollider::Collide(const RectangleCollider& other)const
{
	CollisionTest colTest;
	if (colTest.CollideRectAndRect(other, *this))return true;
	return false;
}

bool RectangleCollider::Collide(const CircleCollider& other)const
{
	CollisionTest colTest;
	if (colTest.CollideCircleAndRect(other, *this))return true;
	return false;
}

bool RectangleCollider::Collide(const LineCollider& other)const
{
	CollisionTest colTest;
	if (colTest.CollideLineAndRect(other, *this))return true;
	return false;
}

bool RectangleCollider::Collide(const CapsuleCollider& other)const
{
	CollisionTest colTest;
	if (colTest.CollideCapsuleAndRect(other, *this))return true;
	return false;
}
