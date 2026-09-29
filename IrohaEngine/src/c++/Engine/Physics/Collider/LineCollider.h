#pragma once
#include "Collider.h"

class Collider;

//ラインコライダークラス：線形のコライダークラス。
class LineCollider :public Collider
{
public:
	LineCollider();
	~LineCollider() {}

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

	XMFLOAT2 getVec()const
	{
		return _vec;
	};

	void setVec(XMFLOAT2 vec)
	{
		_vec = vec;
	}

private:

	XMFLOAT2 _vec;
	float _length=64;
};
