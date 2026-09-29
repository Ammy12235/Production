#pragma once
#include <Iroha.h>
class DamagePopUp:public Entity
{
public:
	DamagePopUp(XMFLOAT2 initPos, int damage);
	
	~DamagePopUp()=default;

	void SetOffset(XMFLOAT2 offset) {
		_offset = offset;
	}
	void Update()override;
	void Draw()const override;
private:
	UINT _damage=0;
	float timer = 0.0f;
	float _destroyTime=1.5f;
	float _fadeTime=1.0f;
	int _damageNumTex = 0;

	float alpha = 1;
	XMFLOAT2 _offset = { 0,0 };
	float _addPower = 10;

	float padding = 1;
};
