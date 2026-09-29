#pragma once
#include "Core/Base.h"

//サウンドクラス：音の情報を保持するクラス。
class Sound
{

public:
	Sound() {
		_soundId = 0;
	};
	~Sound() {

		
	};

	int getId()const
	{
		return _soundId;
	}
	std::string getSoundName()const
	{
		return _soundName;
	}

	std::vector<BYTE>& getSoundData()
	{
		return _buffers;
	}

	WAVEFORMATEX& getWaveFormat()
	{
		return waveFormat;
	}

protected:

	friend class SoundFactory;

private:

	bool _isDelete = false;
	std::string _soundName;
	int _soundId=-1;
	WAVEFORMATEX waveFormat;
	std::vector<BYTE> _buffers;

};
