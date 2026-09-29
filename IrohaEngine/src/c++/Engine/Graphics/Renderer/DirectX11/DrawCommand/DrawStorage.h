#pragma once
#include "Core/Base.h"
#include "DrawCommand.h"
#include "Graphics/Renderer/DirectX11/BaseShading.h"
#include "eLayer.h"
#include "eDrawCommand.h"

class DrawCommand;
class BASE_SHADING;

//ドローストレージクラス:描画コマンドの管理、実行をするクラス。描画コマンドの生成も行う。
class DrawStorage
{
public:
	DrawStorage() = default;
	virtual ~DrawStorage() = default;
	bool init(BASE_SHADING* pBaseShading, ID3D11DeviceContext* pDeviceContext);
	void ExecuteDraw();
	void ExecuteDrawCameraUI();
	void setDrawCommand2D(Command_2D* command_2D);
	void setDrawCommand2D_Instanced(Command_2D_Instanced* command_2D_Instanced);
	void setDrawCommand3D(Command_3D* command_3D);
	void setDrawFlag(bool isDraw,eLayer layer);//このレイヤーを描画するか
private:

	BASE_SHADING* _pBaseShading = nullptr;
	ID3D11DeviceContext* _pDeviceContext = nullptr;

	std::array<bool, Layer_Max> _drawFlag;
	std::array<std::vector<std::unique_ptr<DrawCommand>>, Layer_Max> _drawCommands;//描画コマンド群

};
