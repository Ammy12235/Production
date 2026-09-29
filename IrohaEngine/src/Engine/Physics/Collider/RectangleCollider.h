#pragma once
#include "Collider.h"

class Collider;

//レクタングルコライダークラス：矩形型のコライダークラス。
class RectangleCollider :public Collider
{
public:
	RectangleCollider();
	~RectangleCollider() {}

	bool Dispatch(const Collider& other)const override {
		if (other.Collide(*this))return true;
		return false;
	}

	bool Collide(const RectangleCollider& other)const override;
	bool Collide(const CircleCollider& other)const override;
	bool Collide(const LineCollider& other)const override;
	bool Collide(const CapsuleCollider& other)const override;

	virtual void Update()override;
	bool draw()const override;

	void setSize(XMFLOAT2 size)
	{
		_size = size;
	}

	RectF getRect()const
	{
		return _rect;
	};

private:
	void setRect(XMFLOAT2 pos, XMFLOAT2 size)
	{
		_rect =
		{
			pos.y - size.y / 2,
			pos.y + size.y / 2,
			pos.x - size.x / 2,
			pos.x + size.x / 2,

		};
	}
	XMFLOAT2 _size = { 64,64 };
	RectF _rect;
};
