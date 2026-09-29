#pragma once
#include <Iroha.h>

class EnergyOrb :public Entity
{
public:
	EnergyOrb()
	{
		_idInfo._tag = "EnergyOrb";
	}
	virtual ~EnergyOrb() {};
	void Update()override;
	void Draw()const override;
	void SetTargetPlayer(Entity* player,UINT isMain) { //ターゲットとなるプレイヤーをセット。ついでにメインかサブかも判断する
		if (player != nullptr)
		{
			targetPlayer = player;
			isPlayerMain = isMain;
		}
	}
	void SetInitVelocity(XMFLOAT2 vel) { _initVelocity = vel; }//初速をセット
	void SetReachLeftTime(float time) { reachLeftTime = time; }//プレイヤーに届くまでの時間をセット
	void SetOrbEP(UINT ep) {
		_ep = ep; 
		if (ep == 1)size = 1.0f;
		else if (ep == 5)size = 1.3f;
		else if (ep == 10)size = 2.8f;
	}
	bool GetIsPlayerMain()const { return isPlayerMain; }
	UINT GetOrbEP()const { return _ep; }

private:
	UINT size = 1;
	XMFLOAT2 _initVelocity = { 0,0 };//エネルギーオーブの速度
	UINT isPlayerMain = 0;//プレイヤーがメインかどうか。メインなら0,サブなら1。
	float reachLeftTime = 0.0f;//プレイヤーに到達するまでの時間(０秒なら即座にプレイヤーに適用される)
	UINT _ep = 0;
	Entity* targetPlayer = nullptr;//エネルギーオーブの効果を受けるプレイヤー
};
