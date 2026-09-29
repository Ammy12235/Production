#pragma once
#include <Iroha.h>

class DeathZone :public Entity
{
public:
	DeathZone()
	{
		_idInfo._tag = "DeathZone";

		_deathCollider = AddComponent<RectangleCollider>();
		
	}

	DeathZone(XMFLOAT2 pos)
	{
		_idInfo._tag = "DeathZone";

		_deathCollider = AddComponent<RectangleCollider>();
		_position = pos;
	}
	virtual ~DeathZone() {}

	void Update()override {

	}
	void SetDeathZoneSize(XMFLOAT2 size)
	{
		_deathCollider->setSize(size);
	}

	void ReactionEnter(Entity& other)override;

private:
	std::shared_ptr<RectangleCollider> _deathCollider;
	
};
