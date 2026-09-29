#include "DamagePopUp.h"

DamagePopUp::DamagePopUp(XMFLOAT2 initPos, int damage) :_damage(damage)//表示するダメージを表示する
{
	_position = initPos;
	_damageNumTex = TEX_FAC.CreateTexture("tex/ui/damage_number.png");
	alpha = 1;
	_addPower = 10;
	padding = -5.0f;
};

void DamagePopUp::Update()
{
	timer += FPS.GetFrameSecondTime();
	_addPower *= 0.9f;
	_position.y -= _addPower;
	if (timer > _fadeTime)
	{
		alpha = 1-((timer-_fadeTime) / (_destroyTime - _fadeTime));
	}

	if (timer > _destroyTime)
	{
		Destroy(*this);
	}
}

void DamagePopUp::Draw()const
{
	D3D.SetAlignmentBlendDesc(alpha*255);
	D3D.SetLayer(Layer_WorldUI);
	// 文字数分ループ
	std::string str = std::to_string(_damage);
	UINT textPlace = 10;
	UINT textSize = 30;
	for (int i = 0; i < (int)strlen(str.c_str()); i++)
	{
		//文字コードから表示する文字を特定する(0は0x30から始まるため、0x30オフセットする)
		UINT offset = (str.c_str()[i] - 0x30);
		D3D.DrawDivImage(_damageNumTex, _position.x+_offset.x + (textSize+ padding) * i, _position.y + _offset.y, textSize, textSize, offset * 16, 0, 16, 16);

	}
	D3D.SetLayer(Layer_0);
	D3D.SetDefaultBlendDesc(255);
}
