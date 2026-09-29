#include "CircleCollider.h"
#include "CollisionTest.h"
#include "Core/DEFINE.h"
#include "Core/Entity.h"
#include <IrohaGraphics.h>

CircleCollider::CircleCollider()
{

}

void CircleCollider::Update()
{
	if (_isDelete)return;//削除フラグが立っていたら更新しない
	//位置の更新
	_pos = _entity->GetPosition() + _offset;
	_aabb.x0 = _pos.x - _radius;
	_aabb.y0 = _pos.y - _radius;
	_aabb.x1 = _pos.x + _radius;
	_aabb.y1 = _pos.y + _radius;
}

bool CircleCollider::draw()const
{
	
	if (enabled)D3D.SetAlignmentBlendDesc(200);
	else D3D.SetAlignmentBlendDesc(10);
	int divNum = log(_radius)*5;
	for (int i = 0; i < divNum; i++)
	{
		D3D.DrawLine(_pos.x + _radius * cos(2 * Define::PI / divNum * i), _pos.y + _radius * sin(2 * Define::PI / divNum * i),
			_pos.x + _radius * cos(2 * Define::PI / divNum * ((i + 1) % divNum)), _pos.y + _radius * sin(2 * Define::PI / divNum * ((i + 1) % divNum)), D3D.GetColor(0, 255, 0));
	}
	D3D.SetDefaultBlendDesc(255);
	return true;
}

bool CircleCollider::Collide(const CircleCollider& other)const
{
	CollisionTest colTest;
	if (colTest.CollideCircleAndCircle(other, *this))return true;
	return false;
}

bool CircleCollider::Collide(const RectangleCollider& other)const
{
	CollisionTest colTest;
	if (colTest.CollideCircleAndRect(*this, other))return true;
	return false;
}

bool CircleCollider::Collide(const LineCollider& other)const
{
	CollisionTest colTest;
	if (colTest.CollideLineAndCircle(other, *this))return true;
	return false;
}

bool CircleCollider::Collide(const CapsuleCollider& other)const
{
	CollisionTest colTest;
	if (colTest.CollideCapsuleAndCircle(other, *this))return true;
	return false;
}
