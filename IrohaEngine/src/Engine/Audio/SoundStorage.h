#pragma once

#include "Sound.h"

class Sound;
class ColliderFactory;

//サウンドストレージクラス：サウンドの管理をするクラス。
class SoundStorage
{
public:
	SoundStorage() = default;
	virtual ~SoundStorage() {};


	bool update();//サウンドの更新(音の大きさやミキシング)
	bool draw()const;//サウンドの描画（音の大きさと再生位置）

	int getSoundNum()const;//音の総数を返す
	int getPlayingSoundNum()const;//再生中の音の総数を返す
	int getFreeSlotNum()const;//再生可能なスロットの総数を返す

	void drawColliderDebug()const;

private:
	std::vector<std::shared_ptr<Sound>> _sounds;//サウンド群

	void deleteSound(UINT32 id);


	friend class SoundFactory;
	friend class Sound;
};

