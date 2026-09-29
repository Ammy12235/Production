#pragma once
#include "Core/Singleton.h"
#include "ParticleEmitter.h"
class EngineLoop;

class ParticleSystem :public Singleton<ParticleSystem>
{
private:
	void Init();//パーティクル専用のシェーダを初期化する
	void Update();
	bool FinalizeParticleSystem()
	{

	}

	template<typename CompType>
	void Register(std::shared_ptr<CompType> collider)//ComponentManagerでColliderであることは確認済み
	{
		_particles.push_back(collider);

	}
	void Unregister(UINT32 id);

	// このサブクラスが管理するコンポーネントが持つ一意なID
	//size_t m_physicsCompTypeID=0;
	std::vector<std::shared_ptr<ParticleEmitter>> _particles;//線形に並んだコライダー

	UINT32 calculateNum = 0;
	UINT32 collidingNum = 0;

	friend class ComponentManager;
	friend class EngineLoop;
public:
	ParticleSystem()=default;
	virtual ~ParticleSystem() = default;

	int GetColliderNum()const;//コライダーの総数を返す
	int GetCalculateNum()const;//コライダーの衝突計算の総数を返す
	int GetCollidingNum()const;//コライダーの衝突の総数を返す

	void DrawDebugCollider()const;
	void DrawColliderStatistics()const;

};
