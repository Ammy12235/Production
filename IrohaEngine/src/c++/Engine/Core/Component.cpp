#include "Component.h"
#include "Entity.h"

void IComponent::SetOwner(Entity* entity)
{
	_entity = entity;
}
