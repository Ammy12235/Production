#pragma once

#include <Iroha.h>

class MoveFloor :public Entity
{
private:
	float s = 0;
	XMFLOAT2 _v;
	std::shared_ptr<RectangleCollider> _collider;
public:
	MoveFloor(IdInfo idInfo);
	~MoveFloor(){};

	virtual void Init()override;
	virtual void Update()override;
	virtual void Draw()const override;

	virtual void ReactionColliding(Entity* entity){};//衝突したときのリアクション
	
};
