#pragma once
#include "Core/Singleton.h"

class EngineLoop;
class ComponentManager;
class Collider;

static size_t m_nextPhysicsCompTypeID = 0;//GUID

class PhysicsSystem :public Singleton<PhysicsSystem>
{
private:
	void Update();
	bool ExecuteCollision();
	void Cleanup();
	void CleanupAll();
	bool FinalizePhysicsSystem();

	template<typename CompType>
	void Register(std::shared_ptr<CompType> collider)//ComponentManagerでColliderであることは確認済み
	{
		_colliders.emplace_back(collider);

	}
	void Unregister(UINT32 id);

	// このサブクラスが管理するコンポーネントが持つ一意なID
	//size_t m_physicsCompTypeID=0;
	std::vector<std::shared_ptr<Collider>> _colliders;//線形に並んだコライダーのポインター

	UINT32 calculateNum = 0;
	UINT32 collidingNum = 0;

	friend class ComponentManager;
	friend class EngineLoop;
public:
	PhysicsSystem();
	virtual ~PhysicsSystem();

	int GetColliderNum()const;//コライダーの総数を返す
	int GetCalculateNum()const;//コライダーの衝突計算の総数を返す
	int GetCollidingNum()const;//コライダーの衝突の総数を返す

	void DrawDebugCollider()const;
	void DrawColliderStatistics()const;

	

	template<typename CompType>
	static const size_t GetID()
	{
		static size_t id = [] {
			return ++m_nextPhysicsCompTypeID;
			}();
		return id;
	}
};

