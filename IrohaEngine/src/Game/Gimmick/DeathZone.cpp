#include "DeathZone.h"
#include "Manager/GameManager.h"

void DeathZone::ReactionEnter(Entity& other)
{
	IdInfo idInfo = other.GetIdInfo();
	if (idInfo._tag == "MainPlayer" || idInfo._tag == "SubPlayer")//プレイヤーなら、落ちたのでゲームオーバーにする
	{
		GameManager::GetInstance().SetState(eScoreModeState::ScoreModeState_GameOver);
		Destroy(*this);
	}
	else if (idInfo._tag == "Enemy")//敵なら、消す
	{
		Destroy(other);
	}
}
