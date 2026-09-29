#include "Sugar.h"

Sugar::Sugar(IdInfo idInfo)
{

	_idInfo._tag = "Item";

	texId = TEX_FAC.CreateTexture("tex/item/sugar.png");
	_collider = AddComponent<RectangleCollider>();
	Init();
}

Sugar::~Sugar()
{

}

void Sugar::Init()
{

}

void Sugar::Update()
{
	if (abs(_v.x) < 0.5f)_v.x = 0;
	if (abs(_v.x) > 0)_v.x += 0.02f * _v.x;
	else angle = 0;

	bool isHitSlope = false;
	TerrainSystem::GetInstance().GetStage()->collideStage(*this, _hitInfo, &isFloatingAir, &isHitSlope, width, height);
	ReactToMapchip(_hitInfo, isHitSlope);
}

void Sugar::Draw()const
{
	D3D.SetAlignmentBlendDesc(255);
	D3D.DrawRotImage(texId, _position.x + width / 2, _position.y + height / 2, width, height, angle, 1);
	D3D.SetDefaultBlendDesc(255);
}

void Sugar::ReactToMapchip(hitInfo info, bool isHitSlope)
{
	if (isHitSlope)
	{
		_v.x *= 0.9;
	}

	if (info.isHitLeft)
	{
	}
	if (info.isHitRight)
	{
	}
	if (info.isHitTop)
	{
		_v.x *= 0.9;
	}
	if (info.isHitBottom)
	{
	}
}

void Sugar::ReactionColliding(Entity& other) {

	IdInfo idinfo = other.GetIdInfo();
	XMFLOAT2 otherPos = other.GetPosition();
	XMFLOAT2 otherPrePos = other.GetPrePosition();
	XMFLOAT2 otherV = other.GetVelocity();
	/*
	if (idinfo._tag == "Enemy")
	{
		if (abs(_v.x) > 5)
		{
			_v.x /= 2;
			other.SetVelocity(XMFLOAT2(otherV.x + _v.x, otherV.y));
			_v.x *= -1;
		}
		if (abs(_v.y) > 5)
		{
			_v.y /= 2;
			other.SetVelocity(XMFLOAT2(otherV.x, otherV.y + _v.y));
			_v.y *= -1;
		}
		_position += (_position - otherPos + 0.01f) / 30;
	}
	else if (idinfo._tag == "Item")
	{
		if (idinfo._id == 2)
		{
			if (abs(_v.x) > 5)
			{
				_v.x /= 2;
				other.SetVelocity(XMFLOAT2(otherV.x + _v.x, otherV.y));
				_v.x *= -1;
			}
			if (abs(_v.y) > 5)
			{
				_v.y /= 2;
				other.SetVelocity(XMFLOAT2(otherV.x, otherV.y + _v.y));
				_v.y *= -1;
			}
			_position += (_position - otherPos + 0.01f) / 30;
		}
	}

	*/
}
