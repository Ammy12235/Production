#pragma once
#include "Singleton.h"
#include "Component.h"
#include "Physics/Collider/PhysicsSystem.h"
#include "Fluid/FluidSimulationSystem.h"
#include "Graphics/Renderer/DirectX11/DIRECTWRITE.h"
#include "Graphics/Renderer/DirectX11/DIRECT3D11.h"

class EngineLoop;
class Entity;
class Component;

class Collider; class PhysicsSystem;
class Fluid; class FluidSimulationSystem;

class MapChipDetector;

// ComponentArrayのインターフェースクラス
class IComponentArray
{
public:
	virtual void Update() {}

	virtual void Cleanup() {}

	virtual void CleanupAll() {}
};

static size_t m_nextCompTypeID = 0;//GUID

// コンポーネントを種類ごとに管理するコンテナクラス
template<typename CompType>
class ComponentArray :public IComponentArray
{
public:

	ComponentArray()
	{
		m_vComponents.reserve(10000);
	}

	// このクラスが管理するコンポーネントの更新関数を全て実行する
	void Update()override
	{
		for (auto& comp : m_vComponents)
		{
			comp->Update();
		}
	}

	void Cleanup()override
	{

		//並び変えた後に削除する
		m_vComponents.erase(
			std::remove_if(m_vComponents.begin(), m_vComponents.end(),
				[](const std::shared_ptr<CompType>& component)
				{
					return component->_isDelete;//管理しているコンポーネントの削除フラグが立っていたら削除する。

				}
			),
			m_vComponents.end()
		);

	}

	void CleanupAll()override
	{
		m_vComponents.clear();
	}

	// コンポーネントを追加し、追加したコンポーネントを返す
	std::shared_ptr<CompType> AddComponent()
	{
		m_vComponents.emplace_back(std::make_shared<CompType>());
		return m_vComponents.back();
	}

private:
	std::vector<std::shared_ptr<CompType>> m_vComponents;
	// このコンテナクラスが管理するコンポーネントが持つ一意なID
	static size_t m_compTypeID;
public:
	// CompTypeIDを取得する関数
	static const size_t GetID()
	{
		// この関数を初めて読んだ時にIDを発行
		if (!m_compTypeID)
		{
			m_compTypeID = ++m_nextCompTypeID;//全システム内で唯一のカウンタをインクリメントし、一意に決まるIDを発行する。このコンポーネント群はn番であるというような紐づけ
		}
		return m_compTypeID;
	}
};


template<typename CompType>
size_t ComponentArray<CompType>::m_compTypeID = 0;

// コンポーネントを管理するクラス
class ComponentManager :public Singleton<ComponentManager>
{
public:
	ComponentManager() {}
	virtual ~ComponentManager() {}
	// 全てのコンポーネントの更新関数を実行する
	void Update()
	{
		for (auto& pair : m_umTypeToCompArray)
		{
			pair.second->Update();
		}
	}

	//全てのコンポーネントの削除関数を実行する
	void Cleanup()
	{

		for (auto& pair : m_umTypeToCompArray)
		{
			pair.second->Cleanup();
		}

	}

	void CleanupAll()
	{
		for (auto& pair : m_umTypeToCompArray)
		{
			pair.second->CleanupAll();
		}
	}

	template<typename CompType>
	std::shared_ptr<CompType> AddComponent(std::vector<std::shared_ptr<IComponent>>& vComp)
	{
		std::shared_ptr<CompType> comp;

		AllocateComponent(comp, vComp);

		return comp;

	}


	template<typename CompType>
	void AllocateComponent(std::shared_ptr<CompType>& comp, std::vector<std::shared_ptr<IComponent>>& vComp)
	{
		//=======================================================
		// コライダーなどのサブシステムが必要なコンポーネントは別で登録
		//=======================================================
		if constexpr (std::is_base_of_v<Collider, CompType>)//コライダーなのかどうか
		{
			size_t type = PhysicsSystem::GetInstance().GetID<CompType>();//コンポーネントがインデックス何番か

			comp = std::make_shared<CompType>();
			PhysicsSystem::GetInstance().Register(comp);//サブシステムに登録

			if (vComp.size() <= type)//今の要素数をみて、必要なIDよりも小さいサイズなら
			{
				vComp.resize(type + 1, nullptr);
			}
			vComp[type] = comp;
		}
		else if constexpr (std::is_base_of_v<Fluid, CompType>)//流体シミュレーションモジュールなのかどうか
		{
			size_t type = FluidSimulationSystem::GetInstance().GetID<CompType>();//コンポーネントがインデックス何番か

			comp = std::make_shared<CompType>();
			FluidSimulationSystem::GetInstance().Register(comp);//サブシステムに登録

			if (vComp.size() <= type)//今の要素数をみて、必要なIDよりも小さいサイズなら
			{
				vComp.resize(type + 1, nullptr);
			}
			vComp[type] = comp;
		}
		//=======================================================
		// サブシステムを必要としないコンポーネントはこのマネージャー内で登録(メモリ上では一列に整列されるため高速)
		//=======================================================
		else
		{
			size_t type = ComponentArray<CompType>::GetID();//コンポーネント列がインデックス何番か
			if (m_umTypeToCompArray.find(type) == m_umTypeToCompArray.end())//コンポーネントマネージャが管理しているコンポーネント列になかったら
			{
				m_umTypeToCompArray[type] = std::make_shared<ComponentArray<CompType>>();//新しくその型を管理するコンポーネント列を作る。
			}
			comp = std::static_pointer_cast<ComponentArray<CompType>>(m_umTypeToCompArray[type])->AddComponent();//コンポーネント列にコンポーネントを追加する

			if (vComp.size() <= type)//今の要素数をみて、必要なIDよりも小さいサイズなら
			{
				vComp.resize(type + 1, nullptr);
			}
			vComp[type] = comp;
		}
	}

	friend class EngineLoop;

private:
	void DrawComponentStatistics()
	{
		if (Define::Debug)
		{
			WCHAR str[128];

			size_t num = m_umTypeToCompArray.size();
			swprintf(str, 128, L"ComponentNum:%zd", num);;//存在するコンポーネントの総数
			DWRITE.DrawFormatText(str, 0, 90, 300, 100, D3D.GetColor(255, 255, 255), 1, 1);


		}
	}
	std::unordered_map<size_t, std::shared_ptr<IComponentArray>> m_umTypeToCompArray;//コンポーネント配列をIDで管理するマップ
	
};
