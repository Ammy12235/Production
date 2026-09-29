#pragma once

#include "Core/Base.h"
#include "Core/Component.h"

class IComponent;
class ComponentManager;
class Entity;

class Fluid:public IComponent
{
public:
	Fluid() = default;
	virtual ~Fluid()=default;

	virtual void Update()override {};
};
