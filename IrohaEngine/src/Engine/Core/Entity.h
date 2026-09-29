#pragma once
#include "Base.h"
#include "DEFINE.h"
#include "ComponentManager.h"

#include "Physics/Collider/PhysicsSystem.h"
#include "Fluid/FluidSimulationSystem.h"
#include "EntityManager.h"

class EntityManager;
class Collider;
class PhysicsSystem;
class FluidSimulationSystem;
class ComponentManager;

struct IdInfo
{
	UINT32 _id;            //IDを保存
	std::string _tag;  //タグを保存
	XMFLOAT2 _initPosition;       //初期座標を格納

	IdInfo(UINT32 id, std::string tag, XMFLOAT2 initPos)
	{
		_id = id; _tag = tag;
		_initPosition = initPos;
	}

	IdInfo()
	{
		_id = 0; _tag = "Default";
		_initPosition = XMFLOAT2(0, 0);
	}

};

//エンティティクラス：ゲーム内のあらゆる「もの」の基底クラス。コンポーネントをつけたり、リソースを使ったりして様々な挙動を作っていく。
class Entity
{
public:
	Entity();//初期化において最低限必要
	virtual ~Entity() {}

	virtual void Init() {}//初期化
	virtual void Update() = 0;//更新
	virtual void LateUpdate() {}//更新後の処理
	virtual void Draw()const {}//描画
	virtual void OnDestroy() {}


	//コンポーネントを追加する。内部でComponentManagerにコンポーネントを追加し、このクラスと紐づける
	template<typename CompType>
	std::shared_ptr<CompType> AddComponent()
	{
		static_assert(std::is_base_of_v<IComponent, CompType>,//コンポーネントインターフェースを継承しているかどうか
			"T must be an integral type");

		std::shared_ptr<CompType> pComp;
		if constexpr (std::is_base_of_v<Collider, CompType>)//コライダーなのかどうか
		{
			pComp = ComponentManager::GetInstance().AddComponent<CompType>(m_physicsTypeToComp);//ポインター配列を渡して、マネージャーにコンポーネントを追加してもらう。
		}
		else if constexpr (std::is_base_of_v<Fluid, CompType>)//流体シミュレーションモジュールなのかどうか
		{
			pComp = ComponentManager::GetInstance().AddComponent<CompType>(m_fluidTypeToComp);//ポインター配列を渡して、マネージャーにコンポーネントを追加してもらう。
		}
		else
		{
			pComp = ComponentManager::GetInstance().AddComponent<CompType>(m_vTypeToComp);//同じくここでコンポーネントが返される。どこに登録されるかはComponentManagerのAddComponent関数の中で決められる
		}
		pComp->SetOwner(this);//コンポーネントに自分のポインタを渡す
		return pComp;
	}

	//このエンティティが持っているコンポーネントを返す。コンポーネントが存在しない場合はnullptrを返す。
	template<typename CompType>
	std::shared_ptr<CompType> GetComponent()
	{
		if constexpr (std::is_base_of_v<Collider, CompType>)//コライダーなのかどうか
		{
			return std::dynamic_pointer_cast<CompType>(m_physicsTypeToComp[PhysicsSystem::GetInstance().GetID()]);
		}
		else if constexpr (std::is_base_of_v<Fluid, CompType>)//流体シミュレーションモジュールなのかどうか
		{
			return std::dynamic_pointer_cast<CompType>(m_fluidTypeToComp[FluidSimulationSystem::GetInstance().GetID()]);
		}
		else
		{
			return std::dynamic_pointer_cast<CompType>(m_vTypeToComp[ComponentArray<CompType>::GetID()]);//発行した時のIDとコンポーネント列は一対一で紐づけられているため、目的の型を取り出せる。
		}

		return nullptr;
	}

	//エンティティの実体化。Entityを継承したクラスであれば、どんなクラスでも実体化できる。
	template<typename T, typename... Args>
	std::shared_ptr<T> Instantiate(Args&&... args)
	{
		static_assert(std::is_base_of_v<Entity, T>,//実体化可能なのはEntityを継承したクラスだけ
			"T must be an integral type");


		std::shared_ptr<T> entity = std::make_shared<T>(std::forward<Args>(args)...);
		EntityManager::GetInstance().AddEntity(entity);//エンティティマネージャーへ追加
		return entity;
	}


	//このエンティティが持っていたすべてのコンポーネントに対して削除フラグを立てる。各Managerの方でこのフラグを見て、実際の削除処理を行う。
	static void Destroy(Entity& entity);


	//ゲッター、セッター群
	virtual void SetGravity(bool flag) { _isGravity = flag; };//重力を適用するか
	virtual void SetTerminalVelocity(float terminalVel) { _terminalVelocity = terminalVel; };//終端速度をセット
	virtual void SetPosition(XMFLOAT2 pos) { _position = pos; }//位置をセット
	virtual void SetVelocity(XMFLOAT2 vel) { _velocity = vel; }//速度をセット
	virtual void SetRotation(float rot) { _rotationZ = rot; }//回転をセット
	virtual void SetScale(XMFLOAT2 scl) { _scale = scl; }//スケールをセット

	virtual XMFLOAT2 GetPosition()const { return _position; }//位置を返す
	virtual XMFLOAT2 GetPrePosition()const { return _prePosition; }//1フレーム前の位置を返す
	virtual XMFLOAT2 GetVelocity()const { return _velocity; }//速度を返す
	virtual float GetRotation()const { return _rotationZ; }//回転を返す
	virtual XMFLOAT2 GetScale() { return _scale; }//スケールを返す
	virtual IdInfo GetIdInfo()const { return _idInfo; }//自分の情報を返す
	virtual bool IsDelete()const { return _isDelete; }//削除フラグを返す

protected:

	virtual void ReactionEnter(Entity& other) {};//衝突した瞬間
	virtual void ReactionColliding(Entity& other) {};//衝突しつづけていたら
	virtual void ReactionExit(Entity& other) {};//衝突し終わったら

	IdInfo _idInfo;//ID情報
	XMFLOAT2 _position = { 0,0 };//位置

private:

	//物理計算に使用されるパラメータ
	XMFLOAT2 _velocity = { 0,0 };//速度
	float _terminalVelocity = 20.0f;//終端速度

	float _rotationZ = 0;
	XMFLOAT2 _scale = { 64,64 };

	XMFLOAT2 _prePosition = { 0,0 };//前の位置


	bool InitPhysics();//位置、速度、加速度の初期化
	void UpdatePhysics();//物理的変数の更新をする関数
	void DrawDebugEntity();

	bool _isDelete = false;//削除フラグ
	bool _isGravity = false;//重力の影響を受けるか


	std::vector <std::shared_ptr<IComponent>> m_vTypeToComp;//自身が保持するコンポーネント群（サブシステムを持たない、新しく追加した要素も含む）
	std::vector <std::shared_ptr<IComponent>> m_physicsTypeToComp;//コライダー専用コンポーネント群
	std::vector <std::shared_ptr<IComponent>> m_fluidTypeToComp;//流体シミュレーション専用コンポーネント群
	std::vector<std::weak_ptr<Entity>> children;//子オブジェクト

	friend class EntityManager;
	friend class PhysicsSystem;
};
