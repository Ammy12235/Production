#include "TextButton.h"
#include "Core/Input/Mouse.h"
#include "Graphics/Renderer/DirectX11/DIRECT3D11.h"
#include "Graphics/Renderer/DirectX11/DIRECTWRITE.h"

TextButton::TextButton(unsigned int Id, const WCHAR* text, Rect rect) :p_Text(text), Id(Id)
{
	memcpy_s(&m_Rect, sizeof(m_Rect), &rect, sizeof(rect));
}

bool TextButton::setMousePoint(POINT* p)
{
	point = p;
	return true;
}

bool TextButton::checkCollideMouseCursor()//カーソルとの衝突判定
{
	if (isSelectEnable)
	{

		// マウス座標から選択、押下されているかを判定
		if (point->x > m_Rect.x && point->x<m_Rect.x + m_Rect.w &&
			point->y> m_Rect.y && point->y < m_Rect.y + m_Rect.h)
		{
			isSelected = true;
			return true;
		}
		else
		{
			isSelected = false;
			return false;

		}
	}
	else
	{
		isSelected = false;
	}
}

bool TextButton::update()//選択後の処理を行う
{
	checkCollideMouseCursor();

	if (isSelected && Mouse::GetInstance().GetMouseDown(0))
	{
		isPushed = true;
	}
	else isPushed = false;

	if (isSelected)//継承要検討
		color[1] = 20;
	else
	{
		color[1] = 255;
	}

	return true;
}

bool TextButton::draw()const
{
	DWRITE.DrawFormatText(p_Text, m_Rect.x, m_Rect.y, m_Rect.w, m_Rect.h, D3D.GetColor(color[0], color[1], color[2]), alpha, 1);
	if (isSelected)
		DWRITE.DrawFormatText(L"O", m_Rect.x - 20, m_Rect.y, 30, 30, D3D.GetColor(color[0], color[1], color[2]), alpha, 1);

	return true;
}

int TextButton::getButtonState()const
{
	if (isPushed && isSelected)return 2;
	else if (isSelected)return 1;
	else return 0;
}

bool TextButton::setButtonState(int value,bool isSelectEnable)
{
	if (value > 3 || value < 0)value = 0;
	switch (value)
	{
	case 0:
		isPushed = false;
		isSelected = false;
		break;
	case 1:
		isPushed = false;
		isSelected = true;
		break;
	case 2:
		isPushed = true;
		isSelected = true;
		break;
	default:
		break;
	}

	if (isSelected)//継承要検討
		color[1] = 20;
	else
	{
		color[1] = 255;
	}
	return true;
}
