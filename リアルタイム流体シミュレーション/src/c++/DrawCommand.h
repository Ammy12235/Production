#pragma once
#include "Base.h"
#include "DrawParameter.h"

class DrawCommand
{
public:
	virtual void Execute();
	DrawCommand() =default;
	virtual ~DrawCommand() =default;
};

class PointCommand:public DrawCommand
{
private:
	XMFLOAT3 pos;
	XMFLOAT3 color;
public:
	void Execute()override;
	PointCommand() = default;
	virtual ~PointCommand() = default;

};

class LineCommand :public DrawCommand
{
private:
	XMFLOAT3 pos1;
	XMFLOAT3 pos2;
	XMFLOAT3 color;
public:
	void Execute()override;
	LineCommand() = default;
	virtual ~LineCommand() = default;
};

class RectCommand :public DrawCommand
{
private:
	XMFLOAT3 pos;
	float width, height;
	XMFLOAT3 color;
	eShaderType type;
public:
	void Execute()override;
	RectCommand() = default;
	virtual ~RectCommand() = default;
};

class CubeCommand :public DrawCommand
{
private:
	XMFLOAT3 pos;
	XMFLOAT3 color;
	eShaderType type;
public:
	void Execute()override{};
	CubeCommand() = default;
	virtual ~CubeCommand() = default;
};

class ModelCommand :public DrawCommand
{
private:
	XMFLOAT3 pos;
	XMFLOAT3 color;
	eShaderType type;
public:
	void Execute()override{};
	ModelCommand() = default;
	virtual ~ModelCommand() = default;
};
