#include "EnergyOrb.h"
#include "Player/Player.h"
#include "Player/SubPlayer.h"

void EnergyOrb::Update()
{
	//位置を更新する
	if (targetPlayer != nullptr)
	{

		XMFLOAT2 playerPos = targetPlayer->GetPosition();
		XMFLOAT2 diff = XMM::Normalize(playerPos - _position) * 10;

		XMFLOAT2 totalVelocity = GetVelocity();
		totalVelocity = diff + _initVelocity;
		_initVelocity *= 0.9f;//初速は徐々に減衰させる
		SetVelocity(totalVelocity);
		reachLeftTime -= FPS.GetFrameSecondTime();


		if (XMM::Distance(playerPos, _position) < 10)//プレイヤーにエネルギーオーブの効果を与える処理を行う
		{
			//メインプレイヤーにエネルギーオーブの効果を与える処理
			//プレイヤーのEPを回復する
			if (targetPlayer->GetIdInfo()._tag == "MainPlayer")
			{
				Player* player = dynamic_cast<Player*>(targetPlayer);
				if (player != nullptr)
				{
					player->AddMainEP(_ep);
				}
			}
			else if (targetPlayer->GetIdInfo()._tag == "SubPlayer")
			{
				SubPlayer* subPlayer = dynamic_cast<SubPlayer*>(targetPlayer);
				if (subPlayer != nullptr)
				{
					subPlayer->AddSubEP(_ep);
				}
			}

			targetPlayer = nullptr;//効果を受けるプレイヤーをリセット
			Destroy(*this);
		}
	}
}

void EnergyOrb::Draw()const
{
	D3D.SetAddBlendDesc(255);
	D3D.DrawRotBox(_position.x, _position.y, 10, 10, 0, size, D3D.GetColor(25, 255, 25));
	D3D.SetDefaultBlendDesc(255);
}
