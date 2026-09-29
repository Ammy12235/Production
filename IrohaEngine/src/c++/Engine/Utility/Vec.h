/**
* @file Vec.h
* @brief 2Dの情報を保存できる構造体の宣言
*/
#ifndef VEC_H_
#define VEC_H_

//=====================================================================//
//! Vector用構造体(2D版)
//=====================================================================//
struct Vec2
{
	/** Constructor */
	Vec2()
	{
		X = 0.0f;
		Y = 0.0f;
	}

	/**
	* @brief Constructor
	* @param[in] x 横幅
	* @param[in] y 縦幅
	*/
	Vec2(float x, float y)
	{
		X = x;
		Y = y;
	}

	/**
	* @brief Constructor
	* @param[in] Vec2 コピー用Vec2データ
	*/
	Vec2(const Vec2& size)
	{
		this->X = size.X;
		this->Y = size.Y;
	}

	float X;	//!< X値
	float Y;	//!< Y値
};

struct Vec2I
{
	/** Constructor */
	Vec2I()
	{
		X = 0;
		Y = 0;
	}

	/**
	* @brief Constructor
	* @param[in] x x初期値
	* @param[in] y y初期値
	*/
	Vec2I(int x, int y)
	{
		X = x;
		Y = y;
	}

	/**
	* @brief Constructor
	* @param[in] Vec2I コピー用Vec2Iデータ
	*/
	Vec2I(const Vec2I& vec)
	{
		this->X = vec.X;
		this->Y = vec.Y;
	}

	int X;	//!< X値
	int Y;	//!< Y値
};


#endif
