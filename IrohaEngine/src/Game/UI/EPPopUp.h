#pragma once
#include <Iroha.h>
class EPPopUp :public Entity
{
public:
	EPPopUp(XMFLOAT2 initPos, int value,bool isPlus);

	~EPPopUp() = default;

	void SetOffset(XMFLOAT2 offset) {
		_offset = offset;
	}
	void Update()override;
	void Draw()const override;
private:
	UINT _ep = 0;
	float timer = 0.0f;
	float _destroyTime = 1.5f;
	float _fadeTime = 1.0f;
	int _epNumTex = 0;
	bool _isPlus = false;
	float alpha = 1;
	XMFLOAT2 _offset = { 0,0 };
	float _addPower = 10;
};
