#include "EnemyEraser.h"
#include "Manager/GameManager.h"

void EnemyEraser::ReactionEnter(Entity& other)
{
	IdInfo idInfo = other.GetIdInfo();
	if (idInfo._tag == "Enemy")//敵なら、消す
	{
		Destroy(other);
	}
}
