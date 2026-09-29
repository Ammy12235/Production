#include "CapsuleCollider.h"
#include "CollisionTest.h"
#include "Core/DEFINE.h"
#include "Core/Entity.h"
#include <IrohaGraphics.h>

CapsuleCollider::CapsuleCollider()
{

}

void CapsuleCollider::Update()
{
	if (_isDelete)return;//削除フラグが立っていたら更新しない
	//位置の更新
	_pos = _entity->GetPosition() + _offset;
	float diffX = _length / 2 * abs(cos(_entity->GetRotation())) + _radius;
	float diffY = _length / 2 * abs(sin(_entity->GetRotation())) + _radius;
	_aabb.x0 = _pos.x - diffX;
	_aabb.y0 = _pos.y - diffY;
	_aabb.x1 = _pos.x + diffX;
	_aabb.y1 = _pos.y + diffY;
}

bool CapsuleCollider::draw()const
{

	if(enabled)D3D.SetAlignmentBlendDesc(200);
	else D3D.SetAlignmentBlendDesc(10);
	int divNum = log(_radius) * 5;
	for (int i = 0; i < divNum; i++)
	{
		D3D.DrawLine(_pos.x + _radius * cos(2 * Define::PI / divNum * i), _pos.y + _radius * sin(2 * Define::PI / divNum * i),
			_pos.x + _radius * cos(2 * Define::PI / divNum * ((i + 1) % divNum)), _pos.y + _radius * sin(2 * Define::PI / divNum * ((i + 1) % divNum)), D3D.GetColor(0, 255, 0));
	}
	D3D.SetDefaultBlendDesc(255);
	return true;
}

bool CapsuleCollider::Collide(const CapsuleCollider& other)const
{
	CollisionTest colTest;
	if (colTest.CollideCapsuleAndCapsule(other, *this))return true;
	return false;
}

bool CapsuleCollider::Collide(const RectangleCollider& other)const
{
	CollisionTest colTest;
	if (colTest.CollideCapsuleAndRect(*this, other))return true;
	return false;
}

bool CapsuleCollider::Collide(const CircleCollider& other)const
{
	CollisionTest colTest;
	if (colTest.CollideCapsuleAndCircle(*this, other))return true;
	return false;
}

bool CapsuleCollider::Collide(const LineCollider& other)const
{
	CollisionTest colTest;
	if (colTest.CollideCapsuleAndLine(*this, other))return true;
	return false;
}
