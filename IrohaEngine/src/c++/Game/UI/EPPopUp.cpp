#include "EPPopUp.h"

EPPopUp::EPPopUp(XMFLOAT2 initPos, int value, bool isPlus)
{
	_position = initPos;
	_epNumTex = TEX_FAC.CreateTexture("tex/ui/ep_number.png");
	alpha = 1;
	_addPower = 10;
	_isPlus = isPlus;
	_ep = value;
};

void EPPopUp::Update()
{
	timer += FPS.GetFrameSecondTime();
	_addPower *= 0.9f;
	_position.y -= _addPower;
	if (timer > _fadeTime)
	{
		alpha = 1 - ((timer - _fadeTime) / (_destroyTime - _fadeTime));
	}

	if (timer > _destroyTime)
	{
		Destroy(*this);
	}
}

void EPPopUp::Draw()const
{
	D3D.SetAlignmentBlendDesc(alpha * 255);
	D3D.SetLayer(Layer_WorldUI);
	// 文字数分ループ
	std::string str = std::to_string(_ep);
	UINT textPlace = 10;
	UINT textSize = 20;
	for (int i = 0; i < (int)strlen(str.c_str()); i++)
	{
		//文字コードから表示する文字を特定する(0は0x30から始まるため、0x30オフセットする)
		UINT offset = (str.c_str()[i] - 0x30);
		D3D.DrawDivImage(_epNumTex, _position.x + _offset.x + textSize * i, _position.y + _offset.y, textSize, textSize, offset * 6, 0, 6, 8);

	}
	if (_isPlus)
	{
		D3D.DrawDivImage(_epNumTex, _position.x + _offset.x -16, _position.y + _offset.y, textSize, textSize, 66, 0, 6, 8);
	}
	else
	{
		D3D.DrawDivImage(_epNumTex, _position.x + _offset.x  - 16, _position.y + _offset.y, textSize, textSize, 60, 0, 6, 8);
	}
	D3D.SetLayer(Layer_0);
	D3D.SetDefaultBlendDesc(255);
}
