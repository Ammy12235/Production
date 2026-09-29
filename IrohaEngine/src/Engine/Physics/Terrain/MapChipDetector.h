#pragma once

#include "Core/Base.h"
#include "Core/Component.h"

//マップチップ検出器クラス：マップチップがあるかを捜査するクラス。マップチップの辺の位置に座標を調整する機能も持つ。
class MapChipDetector :public IComponent
{
public:
	MapChipDetector()=default;
	~MapChipDetector()=default;

	void Update()override;

	//bool collideStage(hitInfo& hitInfo, bool* isFloating, bool* isHitSlope, float* x, float* y,
		//float* vx, float* vy, float width, float height);
	bool IsHit(int x, int y)const;

};
