#pragma once
#include "Collider.h"

class Collider;

//カプセルコライダークラス：カプセル形のコライダークラス。
class CapsuleCollider :public Collider
{
public:
	CapsuleCollider();
	~CapsuleCollider() {}

	bool Dispatch(const Collider& other)const override {
		if (other.Collide(*this))return true;
		return false;
	}

	virtual bool Collide(const RectangleCollider& other)const override;
	virtual bool Collide(const CircleCollider& other)const override;
	virtual bool Collide(const CapsuleCollider& other)const override;
	virtual bool Collide(const LineCollider& other)const override;

	virtual void Update()override;
	bool draw()const override;

	float getRadius()const
	{
		return _radius;
	};

	float getLength()const
	{
		return _length;
	};

	void setRadius(float radius)
	{
		_radius = radius;
	};

	void setLength(float length)
	{
		_length = length;
	};

	
private:

	float _radius=32;//カプセルの半径
	float _length=64;//カプセルの長さ
};
