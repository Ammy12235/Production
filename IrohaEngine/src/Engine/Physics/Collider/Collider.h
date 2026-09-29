#pragma once

#include "Core/Base.h"
#include "Core/Component.h"

class IComponent;
class ComponentManager;
class Entity;
class PhysicsSystem;

class RectangleCollider;
class CircleCollider;
class LineCollider;
class CapsuleCollider;

//コライダークラス：コライダーの基底クラス。
class Collider :public IComponent
{
public:
	Collider();
	virtual ~Collider() {}

	virtual bool Dispatch(const Collider& other)const = 0;//ダブルディスパッチで相手を特定

	//すべてのコライダーの衝突処理
	virtual bool Collide(const RectangleCollider& other)const = 0;
	virtual bool Collide(const CircleCollider& other)const = 0;
	virtual bool Collide(const LineCollider& other)const = 0;
	virtual bool Collide(const CapsuleCollider& other)const = 0;

	virtual Entity& getEntity()const;//セットしたエンティティを返す


	virtual void Update()override {};
	virtual bool draw()const = 0;//描画

	virtual void setOffset(XMFLOAT2 offset);//コライダーのオフセット値を変更する

	virtual XMFLOAT2 getPos()const;//コライダーの位置を返す
	virtual UINT32 getId()const;

	virtual void Delete();//コライダーを削除する


protected:
	UINT32 _id = 0;//コライダーの一意に決まるID(何十億個も一度に出せないという前提)
	std::unordered_set<UINT32> _othersId;//衝突しているコライダーのID一覧

	//位置情報
	XMFLOAT2 _pos = { 0,0 };
	XMFLOAT2 _offset = { 0,0 };

	//当たり判定のためのAABBの情報
	AABB _aabb;

	static UINT32 _idCounter;

	friend class PhysicsSystem;

private:
	// コライダーが種類ごとに持つ一意なID
	static size_t m_compTypeID;
};
