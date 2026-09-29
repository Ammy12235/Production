#pragma once
#include <Iroha.h>


class EnemyEraser :public Entity
{
public:
	EnemyEraser()
	{
		_idInfo._tag = "EnemyEnemyEraser";

		_deathCollider = AddComponent<RectangleCollider>();

	}

	EnemyEraser(XMFLOAT2 pos)
	{
		_idInfo._tag = "EnemyEnemyEraser";

		_deathCollider = AddComponent<RectangleCollider>();
		_position = pos;
	}
	virtual ~EnemyEraser() {}

	void Update()override {

	}
	void SetEnemyEraserSize(XMFLOAT2 size)
	{
		_deathCollider->setSize(size);
	}

	void SetActivate(bool flag)
	{
		if (_deathCollider)
		{
			_deathCollider->enabled = flag;
		}
	}

	void ReactionEnter(Entity& other)override;

private:
	std::shared_ptr<RectangleCollider> _deathCollider;

};
