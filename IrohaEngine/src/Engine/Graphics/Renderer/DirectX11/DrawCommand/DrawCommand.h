#pragma once
#include "Core/Base.h"
#include "DrawStorage.h"
#include "eBlendState.h"
#include "eLayer.h"
#include "eDrawCommand.h"

class BASE_SHADING;
class Instancing;

//ドローコマンドクラス：描画命令を格納しておくクラス。
class DrawCommand
{
public:
	DrawCommand() = default;
	virtual ~DrawCommand() = default;
	virtual void Execute(BASE_SHADING& pBaseShading, ID3D11DeviceContext& pDeviceContext)=0;
protected:

};

class DrawCommand_2D :public DrawCommand
{
private:
	Command_2D command;
public:
	void Execute(BASE_SHADING& pBaseShading, ID3D11DeviceContext& pDeviceContext)override;
	DrawCommand_2D(Command_2D* command_2D);
	virtual ~DrawCommand_2D() = default;
};



class DrawCommand_2D_Instanced :public DrawCommand
{
private:
	Command_2D_Instanced command;
public:
	void Execute(BASE_SHADING& pBaseShading, ID3D11DeviceContext& pDeviceContext)override;
	DrawCommand_2D_Instanced(Command_2D_Instanced* command_2D_Instanced);
	virtual ~DrawCommand_2D_Instanced() = default;
};


class DrawCommand_3D :public DrawCommand
{
private:
	Command_3D command;
public:
	void Execute(BASE_SHADING& pBaseShading, ID3D11DeviceContext& pDeviceContext)override;
	DrawCommand_3D(Command_3D* command_3D);
	virtual ~DrawCommand_3D() = default;
};


