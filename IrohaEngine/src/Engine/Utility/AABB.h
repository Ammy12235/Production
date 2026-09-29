#include "XMFLOAT_Helper.h"
/**
* @file AABB.h
* @brief AABBの情報を保存できる構造体の宣言
*/
#ifndef AABB_H_
#define AABB_H_

//=====================================================================//
//! AABB用構造体(2D版)
//=====================================================================//
struct AABB
{
	/** Constructor */
	AABB()
	{
		x0=0;
		y0=0;
		x1=0;
		y1 = 0;
	}

	/**
	* @brief Constructor
	* @param[in] x0 最小x
	* @param[in] y0 最小y
	* @param[in] x1 最大x
	* @param[in] y1 最大y
	*/
	AABB(float x0, float y0, float x1, float y1)
	{
		this->x0 = x0;
		this->y0 = y0;
		this->x1 = x1;
		this->y1 = y1;
	}

	/**
	* @brief Constructor
	* @param[in] AABB コピー用AABBデータ
	*/
	AABB(const AABB& aabb)
	{
		this->x0 = aabb.x0;
		this->y0 = aabb.y0;
		this->x1 = aabb.x1;
		this->y1 = aabb.y1;
	}

	float x0;//最小x
	float y0;//最小y
	float x1;//最大x
	float y1;//最大y
};


#endif
