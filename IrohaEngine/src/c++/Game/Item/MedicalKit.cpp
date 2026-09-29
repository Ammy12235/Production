#include "MedicalKit.h"
#include "Player/Player.h"

MedicalKit::MedicalKit(IdInfo idInfo)
{
	_idInfo._tag = "Item";
	texId = TEX_FAC.CreateTexture("tex/item/medicalKit.png");
	_collider = AddComponent<RectangleCollider>();

}

MedicalKit::~MedicalKit()
{
	
}

void MedicalKit::Init()
{

}

void MedicalKit::Update()
{
	
	bool isHitSlope = false;
	TerrainSystem::GetInstance().GetStage()->collideStage(*this,_hitInfo, &isFloatingAir, &isHitSlope, width, height);
	ReactToMapchip(_hitInfo, isHitSlope);

}

void MedicalKit::Draw()const
{
	D3D.SetAlignmentBlendDesc(255);
	D3D.DrawRotImage(texId, _position.x + width / 2, _position.y + height / 2, width, height, 0, 1);
	D3D.SetDefaultBlendDesc(255);
}

void MedicalKit::ReactToMapchip(hitInfo info, bool isHitSlope)
{

	if (info.isHitLeft)
	{
	}
	if (info.isHitRight)
	{
	}
	if (info.isHitTop)
	{
		
	}
	if (info.isHitBottom)
	{
	}
}

void  MedicalKit::ReactionEnter(Entity& other)
{
	IdInfo idinfo = other.GetIdInfo();
	XMFLOAT2 pos = other.GetPosition();
	XMFLOAT2 prePos = other.GetPrePosition();
	
	if (idinfo._tag == "MainPlayer")
	{
		auto player = dynamic_cast<Player*>(&other);
		player->AddHP(healHP);
		_collider.get()->Delete();
	}
}
