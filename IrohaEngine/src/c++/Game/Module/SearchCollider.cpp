#include "SearchCollider.h"

void SearchCollider::ReactionEnter(Entity& other)
{
	if (other.GetIdInfo()._tag == _targetName)
	{
		_target = &other;
		isDiscover = true;
	}
}
