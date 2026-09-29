#include "PhysicsSystem.h"
#include "Core/DEFINE.h"
#include "Core/Entity.h"
#include "Collider.h"
#include <IrohaGraphics.h>

PhysicsSystem::PhysicsSystem()
{

}

PhysicsSystem::~PhysicsSystem()
{

}

bool PhysicsSystem::FinalizePhysicsSystem()
{
	_colliders.clear();
	return true;
}

void PhysicsSystem::CleanupAll()
{
	_colliders.clear();
}

void PhysicsSystem::Unregister(UINT32 id)
{
	for (auto it = _colliders.begin(); it != _colliders.end();)
	{
		if (it->get()->getId() == id)//一致したらそれを削除する
		{
			it = _colliders.erase(it);
			return;
		}
	}
}

void PhysicsSystem::Update()
{
	//座標などの更新
	for (auto& collider : _colliders)
	{
		collider->Update();
	}
}

bool PhysicsSystem::ExecuteCollision()
{
	calculateNum = 0;
	collidingNum = 0;

	//総当たりで当たり判定を行う（空間分割などはせず、各コライダーが判定するか決める)
	size_t sz = _colliders.size();
	int i = 0, j = 0;
	Collider* P;
	Collider* Q;
	if (sz > 1)//コライダーが最低でも2個ないと衝突判定はできない
	{
		try
		{
			for (i = 0; i < sz; i++) {

				P = _colliders.at(i).get();//コライダーが有効でないなら判定しない
				for (j = i + 1; j < sz; j++) {

					Q = _colliders.at(j).get();

					auto itP = P->_othersId.find(Q->_id);//PはすでにQを持っているか？
					auto itQ = Q->_othersId.find(P->_id);//QはすでにPを持っているか？

					if (!P->enabled || !Q->enabled)//互いのどちらかが衝突判定しないフラグが立っているならすでに当たった判定がないかを見てから判定を継続
					{
						if (P->_othersId.end() != itP)//要素が発見されたら
						{
							P->getEntity().ReactionExit(Q->getEntity());
							P->_othersId.erase(itP);//消す
						}

						if (Q->_othersId.end() != itQ)//要素が発見されたら
						{
							Q->getEntity().ReactionExit(P->getEntity());
							Q->_othersId.erase(itQ);//消す
						}
						continue;
					}

					if (P->Dispatch(*Q))//それぞれが衝突していたなら
					{
						//Pのコールバックを決定

						if (P->_othersId.end() == itP) {//無かったらはじめて衝突した

							P->getEntity().ReactionEnter(Q->getEntity());
							P->_othersId.insert(Q->_id);
						}
						else {//そうでなかったらすでに衝突していて衝突中
							P->getEntity().ReactionColliding(Q->getEntity());
						}

						//Qのコールバックを決定

						if (Q->_othersId.end() == itQ) {//無かったらはじめて衝突した

							Q->getEntity().ReactionEnter(P->getEntity());
							Q->_othersId.insert(P->_id);
						}
						else {//そうでなかったらすでに衝突していて衝突中
							Q->getEntity().ReactionColliding(P->getEntity());
						}

						collidingNum++;
					}
					else//このフレームで衝突していなかったら
					{
						if (P->_othersId.end() != itP)//要素が発見されたら
						{
							P->getEntity().ReactionExit(Q->getEntity());
							P->_othersId.erase(itP);//消す
						}

						if (Q->_othersId.end() != itQ)//要素が発見されたら
						{
							Q->getEntity().ReactionExit(P->getEntity());
							Q->_othersId.erase(itQ);//消す
						}
					}

					calculateNum++;
				}
			}
		}
		catch (const std::exception&)
		{
			OutputDebugString(L"PhysicsSystem:Out of Range!!");
		}
	}

	return true;
}

void PhysicsSystem::Cleanup()
{
	//並び変えた後に削除する
	_colliders.erase(
		std::remove_if(_colliders.begin(), _colliders.end(),
			[](const std::shared_ptr<Collider>& collider)
			{
				return collider->_isDelete;

			}
		),
		_colliders.end()
	);

}

void PhysicsSystem::DrawDebugCollider()const
{
	if (Define::Debug)
	{
		for (auto it = _colliders.begin(); it != _colliders.end();)
		{
			(*it)->draw();
			it++;
		}
	}
	
}

//コライダーの総数を返す
int PhysicsSystem::GetColliderNum()const
{
	return _colliders.size();
}

////コライダーの衝突計算の総数を返す
int PhysicsSystem::GetCalculateNum()const
{
	return calculateNum;
}

//コライダーの衝突の総数を返す
int PhysicsSystem::GetCollidingNum()const
{
	return collidingNum;
}


//コライダーの統計を表示する
void PhysicsSystem::DrawColliderStatistics()const
{
	if (Define::Debug)
	{
		WCHAR str[128];

		size_t num = _colliders.size();
		swprintf(str, 128, L"ColliderNum:%zd", num);
		DWRITE.DrawFormatText(str, 0, 130, 300, 100, D3D.GetColor(255, 255, 255), 1, 1);

		num = calculateNum;
		swprintf(str, 128, L"CalculateNum:%zd", num);
		DWRITE.DrawFormatText(str, 0, 160, 300, 100, D3D.GetColor(255, 255, 255), 1, 1);

		num = collidingNum;
		swprintf(str, 128, L"CollidingNum:%zd", num);
		DWRITE.DrawFormatText(str, 0, 190, 300, 100, D3D.GetColor(255, 255, 255), 1, 1);
	}
}
