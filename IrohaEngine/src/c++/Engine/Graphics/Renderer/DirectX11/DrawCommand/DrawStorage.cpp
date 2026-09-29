#include "DrawStorage.h"
#include "DrawCommandUtility.h"

bool DrawStorage::init(BASE_SHADING* pBaseShading, ID3D11DeviceContext* pDeviceContext)
{
	_pBaseShading = pBaseShading;
	_pDeviceContext = pDeviceContext;

	for (int i = 0; i < _drawFlag.size(); i++)
		_drawFlag[i] = true;
	return true;
}

void DrawStorage::setDrawCommand2D(Command_2D* command_2D)
{
	eLayer layer = command_2D->layer;
	_drawCommands[layer].push_back(std::make_unique<DrawCommand_2D>(command_2D));
}

void DrawStorage::setDrawCommand2D_Instanced(Command_2D_Instanced* command_2D_Instanced)
{
	eLayer layer = command_2D_Instanced->layer;
	_drawCommands[layer].push_back(std::make_unique<DrawCommand_2D_Instanced>(command_2D_Instanced));
}

void DrawStorage::setDrawCommand3D(Command_3D* command_3D)
{
	eLayer layer = command_3D->layer;
	_drawCommands[layer].push_back(std::make_unique<DrawCommand_3D>(command_3D));
}

void DrawStorage::setDrawFlag(bool isDraw, eLayer layer)
{
	_drawFlag[layer] = isDraw;
}

void DrawStorage::ExecuteDraw()
{
	
	for (int i = eLayer::Layer_Max - 1; i >= eLayer::Layer_0; i--)
	{
		if (_drawFlag[i] == false)
		{
			_drawCommands[i].clear();
			continue;
		}
		for (auto it = _drawCommands[i].begin(); it != _drawCommands[i].end();)
		{
  			(*it)->Execute(*_pBaseShading, *_pDeviceContext);//描画を実行する
			it++;
		}
	    _drawCommands[i].clear();//描画が実行されたら速やかに描画コマンドを削除する
	}
}

void DrawStorage::ExecuteDrawCameraUI()
{
	for (int i = eLayer::Layer_0 - 1; i >= 0; i--)
	{
		if (_drawFlag[i] == false)
		{
			_drawCommands[i].clear();
			continue;
		}
		for (auto it = _drawCommands[i].begin(); it != _drawCommands[i].end();)
		{
			(*it)->Execute(*_pBaseShading, *_pDeviceContext);//描画を実行する
			it++;
			
		}
		_drawCommands[i].clear();//描画が実行されたら速やかに描画コマンドを削除する
	}
}
