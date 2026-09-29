#pragma once
#include "Core/Base.h"
#include "Graphics/Renderer/DirectX11/BaseShading.h"
#include "eDrawCommand.h"

class BASE_SHADING;

class DrawCommandUtility
{
public:
	void Execute(eDrawCommand drawCommand,BASE_SHADING* pBaseShading, ID3D11DeviceContext* pDeviceContext);
private:

};
