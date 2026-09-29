#pragma once

#include "Core/Base.h"

//データ塊
struct tBlob
{
	LPVOID pData;
	SIZE_T Size;
};

//2D用頂点バッファ
struct Vertex2D
{
	XMFLOAT3 Pos;
	XMFLOAT4 Color;
	XMFLOAT2 Uv;
	Vertex2D() = default;

	Vertex2D(const Vertex2D&) = default;
	Vertex2D& operator=(const Vertex2D&) = default;

	Vertex2D(Vertex2D&&) = default;
	Vertex2D& operator=(Vertex2D&&) = default;
};

//３D用頂点バッファ
struct Vertex3D
{
	XMFLOAT3 Pos;
	XMFLOAT4 Color;
	XMFLOAT3 Nor;
	Vertex3D() = default;
		  
	Vertex3D(const Vertex3D&) = default;
	Vertex3D& operator=(const Vertex3D&) = default;
		  
	Vertex3D(Vertex3D&&) = default;
	Vertex3D& operator=(Vertex3D&&) = default;
};

//インスタンシング２D用頂点バッファ
struct InstanceData2D
{
	XMFLOAT3 pos;
	XMFLOAT2 uvOffset;
};
