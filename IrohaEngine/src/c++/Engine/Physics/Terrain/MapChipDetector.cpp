#include "MapChipDetector.h"
#include "StageUtility.h"

bool MapChipDetector::IsHit(int x, int y)const
{
	/*
	int tx = floor((float)x / Define::ChipSize);
	int ty = floor((float)y / Define::ChipSize);

	if (tx<0 || tx>t_stgInfo.xSize || ty<0 || ty>t_stgInfo.ySize - 1)return false;
	try
	{

		if (mapHitData[ty * t_stgInfo.xSize + tx] > 0)
			return true; else return false;
	}
	catch (const std::exception&)
	{
		OutputDebugString(L"Out of Range!!(isHit)\n");
		return false;
	}

	return true;
	*/
	return true;
};

	/*
bool MapChipDetector::collideStage(hitInfo& hitInfo, bool* isFloating, bool* isHitSlope, float* x, float* y,
	float* vx, float* vy, float width, float height)
{
	hitInfo.reset();
	StageUtility stgUtility;
	if (stgUtility.collide(this, hitInfo, isFloating, isHitSlope, x, y, vx, vy, width, height))return true;



	return false;
}
	*/

void MapChipDetector::Update()
{
	/*
	XMFLOAT2 pos = _entity->GetPosition();
	XMFLOAT2 vel= _entity->GetVelocity();

	collideStage(hitInfo & hitInfo, bool* isFloating, bool* isHitSlope, float* x, float* y,
		float* vx, float* vy, float width, float height)
	*/
}
