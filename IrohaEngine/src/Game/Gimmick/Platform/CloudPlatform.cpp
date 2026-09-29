#include "CloudPlatform.h"

void CloudPlatform::Init()
{
	_idInfo._tag = "Gimmick";
	_collider = AddComponent<RectangleCollider>();
}
void CloudPlatform::Update()
{

}
void CloudPlatform::Draw()const
{

}

void CloudPlatform::ReactionColliding(Entity* entity)//衝突したときのリアクション
{

};
