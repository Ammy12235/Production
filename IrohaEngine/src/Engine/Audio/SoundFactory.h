#pragma once
#include "Core/Singleton.h"
#include "Core/Entity.h"
#include "Sound.h"
#include "SoundEmitter.h"
#include "MediaFoundation.h"

class Entity;
class SoundEmitter;
class Sound;
//サウンドファクトリークラス：音声ファイルからのロード、ミキシングの作成、サウンドエミッターを作成するクラス。音声ファイルはWAV,MP3,Oggを読み込むことができる
class SoundFactory:public Singleton<SoundFactory>
{
public:

	void RemoveDevice();

	// 画像ファイルを読み込む
	int CreateSound(const std::string& filename);//WAV,MP3,Oggを読み込む
	bool UnloadSound(int ID);//サウンドを削除する

	int getIDbyName(std::string key) const;
	std::shared_ptr<Sound> getSoundbyId(int id) const;
	int getSoundNum()const;
	void drawSoundNum()const;

	SoundFactory();
	virtual ~SoundFactory()=default;
private:

	void setSound(std::string key, std::shared_ptr<Sound> sound, int soundId);//作成したサウンドをマップにセットする
	std::vector<std::shared_ptr<Sound>> _sound;
	std::unordered_map<std::string, int> _mapName;//IDと名前を紐づける
	std::unordered_map<int, std::shared_ptr<Sound>> _mapId;//テクスチャをIDで管理する

	MediaFoundation mediaFoundation;

};
#define SND_FAC SoundFactory::GetInstance()

