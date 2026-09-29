#pragma once
#include "Collider.h"

class Collider;

//サークルコライダークラス：円形のコライダークラス。
class CircleCollider :public Collider
{
public:
	CircleCollider();
	~CircleCollider() {}

	bool Dispatch(const Collider& other)const override {
		if (other.Collide(*this))return true;
		return false;
	}

	virtual bool Collide(const RectangleCollider& other)const override;
	virtual bool Collide(const CircleCollider& other)const override;
	virtual bool Collide(const LineCollider& other)const override;
	virtual bool Collide(const CapsuleCollider& other)const override;

	virtual void Update()override;
	bool draw()const override;

	float getRadius()const
	{
		return _radius;
	};

	void setRadius(float radius)
	{
		_radius = radius;
	}


private:

	float _radius=32;
};
