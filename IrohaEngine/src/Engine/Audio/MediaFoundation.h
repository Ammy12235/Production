#pragma once

#include "Core/Base.h"
#include "Sound.h"

class MediaFoundation
{
public:
	MediaFoundation() = default;
	~MediaFoundation();

	bool Init();
	bool LoadSound(const std::wstring path, Sound& sound);
};
