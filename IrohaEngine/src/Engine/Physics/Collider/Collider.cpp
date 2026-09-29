#include "Collider.h"
#include "Core/Entity.h"

UINT32 Collider::_idCounter;

Collider::Collider()
{
	//インクリメントして代入し、IDとする
	_idCounter++;
	_id = _idCounter;
}


Entity& Collider::getEntity()const
{

	return *_entity;

}

XMFLOAT2 Collider::getPos()const {//コライダーの位置を返す
	return _pos;
}

UINT32 Collider::getId()const
{
	return _id;
}


void Collider::setOffset(XMFLOAT2 offset)//コライダーのオフセット値を変更する
{
	_offset = offset;
}



void Collider::Delete()//コライダーを削除する
{
	_isDelete = true;
}

