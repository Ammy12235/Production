#include "Entity.h"
#include "Graphics/Renderer/DirectX11/DIRECT3D11.h"

Entity::Entity()
{

}


bool Entity::InitPhysics()
{
	_velocity = XMFLOAT2(0.0f, 0.0f);//速度
	
	return true;
}

void Entity::UpdatePhysics()
{
	_prePosition = _position;//このフレームの座標を保存してから速度を実際に更新する

	if (XMM::Length(_velocity) > _terminalVelocity)//終端速度にする
	{
		_velocity = XMM::Normalize(_velocity) * _terminalVelocity;
	}
	_position += _velocity;


}

void Entity::DrawDebugEntity()
{
	if (Define::Debug)
	{
		D3D.SetDefaultBlendDesc(255);
		D3D.DrawLine(_position.x - _scale.x / 2, _position.y, _position.x + _scale.x / 2, _position.y, D3D.GetColor(255, 0, 0));
		D3D.DrawLine(_position.x, _position.y - _scale.y / 2, _position.x, _position.y + _scale.y / 2, D3D.GetColor(255, 0, 0));
	}
}

void Entity::Destroy(Entity& entity)
{
	entity.OnDestroy();//デストロイ時に呼ばれる関数を呼ぶ
	for (size_t i = 0; i < entity.m_vTypeToComp.size(); i++)
	{
		if (entity.m_vTypeToComp[i].get() != nullptr)//コンポーネントが存在しているなら
		{
			entity.m_vTypeToComp[i].get()->_isDelete = true;//削除のために削除フラグを立てる(後続のシステムで処理しないようにするため)
		}
	}
	entity.m_vTypeToComp.clear();

	for (size_t i = 0; i < entity.m_physicsTypeToComp.size(); i++)
	{
		if (entity.m_physicsTypeToComp[i].get() != nullptr)//コンポーネントが存在しているなら
		{
			entity.m_physicsTypeToComp[i].get()->_isDelete = true;//コンポーネントが存在しているなら
		}
	}
	entity.m_physicsTypeToComp.clear();

	for (size_t i = 0; i < entity.m_fluidTypeToComp.size(); i++)
	{
		if (entity.m_fluidTypeToComp[i].get() != nullptr)//コンポーネントが存在しているなら
		{
			entity.m_fluidTypeToComp[i].get()->_isDelete = true;//コンポーネントが存在しているなら
		}
	}
	entity.m_fluidTypeToComp.clear();

	entity._isDelete = true;

}
