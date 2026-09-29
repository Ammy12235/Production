#pragma once

//レイヤー定義
enum eLayer
{
	Layer_None,
	Layer_UI,
	Layer_0,
	Layer_1,
	Layer_2,
	Layer_3,
	Layer_4,
	Layer_5,
	Layer_Max
};

//形状定義
enum eShape
{
	Shape_None,
	Shape_Point,
	Shape_Line,
	Shape_Rect,
	Shape_Cube,
	Shape_Model,
	Shape_Max
};

//シェーダータイプ定義
enum eShaderType
{
	ShaderType_None,
	ShaderType_Color,//色指定
	ShaderType_Texture,//テクスチャ指定
	ShaderType_Max
};