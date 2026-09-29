#include "Stage.h"
#include "Core/DEFINE.h"
#include "Core/Camera/Camera.h" 
#include "StageUtility.h"
#include <IrohaGraphics.h>

#define STAGE_MAXSIZE 1024
Stage::Stage() {

}
bool Stage::Load(const LevelData& data)
{
	StageUtility stgUtility;
	//当たり判定の読み込み
	stgUtility.openStgFile(data.stgHitFileName, stageId, mapHitData, t_stgInfo);

	//マップデータと画像の読み込み
	stgUtility.openStgFile(data.stgFileName, stageId, mapData, t_stgInfo);
	texId = TEX_FAC.CreateTexture(data.stgTexFileName);
	maptipXCount = data.texWidthCount;
	maptipYCount = data.texHeightCount;

	int index = 0;
	InstanceData2D instance;
	for (int j = 0; j < t_stgInfo.ySize; j++)
	{
		for (int i = 0; i < t_stgInfo.xSize; i++)
		{
			instance.pos = XMFLOAT3(i * Define::ChipSize, j * Define::ChipSize, 0);
			int tipX = mapData[index] % maptipXCount;
			int tipY = mapData[index] / maptipXCount;
			float u = (float)tipX / maptipXCount;
			float v = (float)tipY / maptipYCount;
			if (mapData[index] < 0)
				instance.uvOffset = XMFLOAT2(0, 0);
			else
				instance.uvOffset = XMFLOAT2(u, v);

			instances.emplace_back(instance);
			index++;
		}
	}
	return true;

}

bool Stage::UnLoad()
{
	instances.clear();
	mapHitData.clear();
	mapData.clear();
	TEX_FAC.DeleteTexture(texId);
	return true;
}
stgInfo Stage::getStageInfo()
{
	return t_stgInfo;
}

std::vector<int> Stage::getStageHitData()
{
	return mapHitData;
}


void Stage::Update()
{
	

}


void Stage::Draw()const
{
	
	
	D3D.SetAlignmentBlendDesc(255);
	
	D3D.SetLayer(Layer_3);
	//D3D.DrawImage(backTexId, 0, 0, 1000, 1000);
	int dataSize = t_stgInfo.xSize * t_stgInfo.ySize;
	D3D.SetLayer(Layer_Stage);

	
	D3D.DrawInstancedImage(texId, 0, 0,
		*instances.data(), instances.size(), 20, 20);
	
	D3D.SetDefaultBlendDesc(255);
}
bool Stage::isHit(int x, int y)const
{
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

};

bool Stage::isHitSlope(int x, int y)const
{

	int tx = floor((float)x / Define::ChipSize);
	int ty = floor((float)y / Define::ChipSize);

	if (tx<0 || tx>t_stgInfo.xSize || ty<0 || ty>t_stgInfo.ySize - 1)return false;
	try
	{

		if (mapHitData[ty * t_stgInfo.xSize + tx] > 1)
			return true; else return false;
	}
	catch (const std::exception&)
	{
		OutputDebugString(L"Out of Range!!(isHitSlope)\n");
		return false;
	}
	return true;
}

bool Stage::collideStage(Entity& entity,hitInfo& hitInfo, bool* isFloating, bool* isHitSlope, float width, float height)
{
	
	hitInfo.reset();
	StageUtility stgUtility;
	if (stgUtility.collide(*this,entity ,hitInfo, isFloating, isHitSlope, width, height))return true;

	return false;
}
