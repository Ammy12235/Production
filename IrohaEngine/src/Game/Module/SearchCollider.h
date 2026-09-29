#pragma once
#include <Iroha.h>

class SearchCollider:public Entity
{
public:
	SearchCollider() {
		_collider = AddComponent<CircleCollider>();

	}
	;
	virtual ~SearchCollider() = default;

	void SetTargetName(std::string targetName)
	{
		_targetName = targetName;
	}

	void SetRadius(float radius)
	{
		_collider->setRadius(radius);
	}

	void ReactionEnter(Entity& other)override;

	void Update()override {}

	bool IsDiscover()
	{
		return isDiscover;
	}

	const Entity* GetDiscoverEntity()
	{
		return _target;
	}

private:

	std::string _targetName;
	Entity* _target;
	bool isDiscover=false;
	std::shared_ptr<CircleCollider> _collider=nullptr;
	

};
