#include "LineCollider.h"
#include "CollisionTest.h"
#include "Core/DEFINE.h"
#include "Core/Entity.h"
#include <IrohaGraphics.h>

LineCollider::LineCollider()
{

	_vec.x = cos(0) * _length;
	_vec.y = sin(0) * _length;

}

void LineCollider::Update()
{
	if (_isDelete)return;//削除フラグが立っていたら更新しない
	//位置の更新
	_pos = _entity->GetPosition() + _offset;
	float rotation = _entity->GetRotation();
	_vec.x = cos(rotation) * _length;
	_vec.y = sin(rotation) * _length;

	_aabb.x0 = min(_pos.x, _pos.x + _vec.x);
	_aabb.y0 = min(_pos.y, _pos.y + _vec.y);
	_aabb.x1 = max(_pos.x, _pos.x + _vec.x);
	_aabb.y1 = max(_pos.y, _pos.y + _vec.y);
}

bool LineCollider::draw()const
{

	if (enabled)D3D.SetAlignmentBlendDesc(200);
	else D3D.SetAlignmentBlendDesc(10);
	for (int i = 0; i < 2; i++)
	{
		D3D.DrawLine(_pos.x, _pos.y,
			_pos.x + _vec.x, _pos.y + +_vec.y, D3D.GetColor(0, 255, 0));
	}
	D3D.SetDefaultBlendDesc(255);
	return true;
}

bool LineCollider::Collide(const LineCollider& other)const
{
	CollisionTest colTest;
	if (colTest.CollideLineAndLine(other, *this))return true;
	return false;
}

bool LineCollider::Collide(const CircleCollider& other)const
{
	CollisionTest colTest;
	if (colTest.CollideLineAndCircle(*this, other))return true;
	return false;
}

bool LineCollider::Collide(const RectangleCollider& other)const
{
	CollisionTest colTest;
	if (colTest.CollideLineAndRect(*this, other))return true;
	return false;
}

bool LineCollider::Collide(const CapsuleCollider& other)const
{
	CollisionTest colTest;
	if (colTest.CollideCapsuleAndLine(other, *this))return true;
	return false;
}

