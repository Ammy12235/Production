#pragma once
/**
* @file Rect.h
* @brief 矩形の情報を保存できる構造体の宣言
*/
#ifndef RECT_H_
#define RECT_H_

//=====================================================================//
//! RectF用構造体(2D版)
//=====================================================================//
struct RectF
{
	/** Constructor */
	RectF()
	{
		Top = 0.0f;
		Bottom = 0.0f;
		Left = 0.0f;
		Right = 0.0f;
	}

	/**
	* @brief Constructor
	* @param[in] top y最小
	* @param[in] bottom y最大
	* @param[in] left x最小
	* @param[in] right x最大
	*/
	RectF(float top, float bottom, float left, float right)
	{
		Top = top;
		Bottom = bottom;
		Left = left;
		Right = right;
	}

	/**
	* @brief Constructor
	* @param[in] rect コピー用RectFデータ
	*/
	RectF(const RectF& rect)
	{
		this->Top = rect.Top;
		this->Bottom = rect.Bottom;
		this->Left = rect.Left;
		this->Right = rect.Right;
	}

	float Top;		//!< 上辺Y座標
	float Bottom;	//!< 下辺Y座標
	float Left;		//!< 左辺X座標
	float Right;	//!< 右辺X座標
};


#endif
