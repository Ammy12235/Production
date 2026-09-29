#pragma once
#include <Iroha.h>

class UpAdvection :public Entity
{
public:
	UpAdvection();
	virtual ~UpAdvection() = default;

	void Update()override;

private:
	std::shared_ptr<FluidInteractAdvection> _fluidAdvection=nullptr;

	float lifeTime = 0.0f;
	float timer = 0.0f;
};
