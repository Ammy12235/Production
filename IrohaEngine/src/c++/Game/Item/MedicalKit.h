#pragma once
#include <Iroha.h>

class MedicalKit:public Entity
{
private:

	UINT healHP = 80;
	XMFLOAT2 _v;
	int texId = 0;
	float width = (64.0f * 1.0f), height = (64.0f * 1.0f);
	bool isDelete = false;
	float g = Define::Gravity;

	// マップチップの辺の位置に座標を調整する
	void ReactToMapchip(hitInfo info, bool isHitSlope);

	void ReactionEnter(Entity& other)override;
	bool isFloatingAir = false;
	hitInfo _hitInfo;

	std::shared_ptr<RectangleCollider> _collider;
public:
	MedicalKit(IdInfo idInfo);
	virtual ~MedicalKit();

	void Init()override;
	void Update()override;
	void Draw()const override;



protected:
};
