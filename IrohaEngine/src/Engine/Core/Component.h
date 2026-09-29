#pragma once

class Entity;

// コンポーネントの基底クラス
class IComponent
{
public:
	IComponent() {}
	virtual ~IComponent() {}

	virtual void Update() {}
	// 持ち主の参照をセットする関数
	void SetOwner(Entity* entity);
	bool enabled = true;//コンポーネントが有効かどうか
	bool _isDelete = false;//削除するか（立ってたら管理部で削除される）
protected:

	Entity* _entity = nullptr;//このコンポーネントを持つエンティティ
};
