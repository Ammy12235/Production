#pragma once

struct LevelData
{
	std::string stgFileName;   //ステージデータのファイル名
	std::string stgHitFileName;//ステージデータのファイル名
	std::string enemyFileName; //敵のデータファイル名
	std::string itemFileName;  //アイテムのデータファイル名

	std::string stgTexFileName;//ステージテクスチャのファイル名
	int texWidthCount, texHeightCount;//テクスチャ分割数
};


struct stgInfo
{
	int stageId;
	int xSize, ySize;

	stgInfo(int stgId, int x, int y)
	{
		stageId = stgId;
		xSize = x;
		ySize = y;
	}

	stgInfo()
	{
		stageId = -1;
		xSize = 0;
		ySize = 0;
	}

};

struct hitInfo
{
	bool isHitLeft = false;
	bool isHitRight = false;
	bool isHitTop = false;
	bool isHitBottom = false;

	void reset()
	{
		isHitLeft = false;
		isHitRight = false;
		isHitTop = false;
		isHitBottom = false;
	}
};

// 辺の種類
enum EdgeType
{
	EdgeTypeNon = -1,
	EdgeTypeLeft,
	EdgeTypeRight,
	EdgeTypeTop,
	EdgeTypeBottom,
	EdgeTypeMax,
};

//矩形の種類
enum RectType
{
	RectTypeNon = -1,
	RectTypeNormal,
	RectTypeSlope,
};

enum Chiptype
{
	Non = -1,
	Air,                      //判定なし
	Rect,                     //矩形
	SlopeUp_Ground,           //地面右上がり45度
	SlopeDown_Ground,         //地面右下がり45度
	SlopeUp_GroundHalf_01,    //地面右上がり22.5度下半分
	SlopeUp_GroundHalf_02,    //地面右上がり22.5度上半分
	SlopeDown_GroundHalf_01,  //地面右下がり22.5度上半分
	SlopeDown_GroundHalf_02,  //地面右下がり22.5度下半分
	SlopeUp_Ceiling,          //天井右上がり45度
	SlopeDown_Ceiling,        //天井右下がり45度
	SlopeUp_CeilingHalf_01,   //天井右上がり22.5度下半分
	SlopeUp_CeilingHalf_02,   //天井右上がり22.5度上半分
	SlopeDown_CeilingHalf_01, //天井右下がり22.5度上半分
	SlopeDown_CeilingHalf_02, //天井右下がり22.5度下半分
};
