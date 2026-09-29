#pragma once
#include "Base.h"
#include "DrawCommand.h"
#include "DrawParameter.h"

class DrawStorage
{
public:
	DrawStorage()=default;
	virtual ~DrawStorage();
private:
	void ExecuteDraw();

	std::array<std::vector<std::unique_ptr<DrawCommand>>,eLayer::Layer_Max> _drawCommands;
	friend class DrawCommand;

};