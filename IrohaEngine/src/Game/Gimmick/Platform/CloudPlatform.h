#pragma once
#include <Iroha.h>

class CloudPlatform :public Entity
{
public:
	CloudPlatform(IdInfo idInfo) {};
	~CloudPlatform() {};

	virtual void Init()override;
	virtual void Update()override;
	virtual void Draw()const override;

	virtual void ReactionColliding(Entity* entity);//衝突したときのリアクション
private:
	std::shared_ptr<RectangleCollider> _collider;
};
