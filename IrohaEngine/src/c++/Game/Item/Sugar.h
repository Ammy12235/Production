#pragma once
#include <Iroha.h>

class Sugar :public Entity
{
private:

	XMFLOAT2 _v;
	int texId = 0;
	float _flyingRegist = 1.0f;
	float width = (64.0f * 1.0f), height = (64.0f * 1.0f);

	bool isDelete = false;
	float g = Define::Gravity;
	float angle = 0;
	// マップチップの辺の位置に座標を調整する
	void ReactToMapchip(hitInfo info, bool isHitSlope);

	//void ReactionEnter(Entity& other)override;
	void ReactionColliding(Entity& other)override;
	//void ReactionExit(Entity& other)override;

	bool isFloatingAir = false;
	hitInfo _hitInfo;
	std::shared_ptr<RectangleCollider> _collider;
public:
	Sugar( IdInfo idInfo);
	virtual ~Sugar();

	void Init()override;
	void Update()override;
	void Draw()const override;



protected:
};
