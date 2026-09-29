#include "SoundFactory.h"
#include "Core/Algorithm/Hash.h"

SoundFactory::SoundFactory()
{
	_mapName.clear();
	_mapId.clear();
	mediaFoundation.Init();//MediaFoundationの初期化
}

void SoundFactory::RemoveDevice()
{

}

// 音声ファイルを読み込む
int SoundFactory::CreateSound(const std::string& filename)
{
	//拡張子を調べる
	const char* pExtension = "";
	for (size_t i = strlen(filename.c_str()); i != 0; i--)
	{
		if (filename[i - 1] == '.')
		{
			pExtension = &filename[i];
		}
	}

	SetDataDirectory();
	// マルチバイト文字列からワイド文字列へ変換
	std::wstring path = ToWide(filename);

	auto it = _mapName.find(filename);//指定キーを取得
	if (_mapName.end() != it)//すでに生成されていたら
		return getIDbyName(filename);//生成せずにIDを渡す

	//配列の後ろに追加していく
	auto sound = std::make_shared<Sound>();//今からデータを詰めるサウンドクラス
	_sound.push_back(sound);
	//========================================================
	// サウンドをデコード
	//========================================================
	// 
	//音声ファイルの読み込み
	if (strcmp(pExtension, "wav") == 0 || strcmp(pExtension, "mp3") == 0)
	{
		mediaFoundation.LoadSound(path, *sound);//バッファにデータを記録
	}
	

	Hash hash(filename.c_str());
	int soundId = hash.GetDigest();
	setSound(filename, sound, soundId);

	// 成功！
	return soundId;
}

void SoundFactory::setSound(std::string key, std::shared_ptr<Sound> sound, int soundId)
{
	//管理側の登録
	_mapName[key] = soundId;
	_mapId[soundId] = sound;

	//サウンド側の登録
	sound->_soundId = soundId;
	sound->_soundName = key;
}


bool SoundFactory::UnloadSound(int ID)
{

	return true;
}

int SoundFactory::getIDbyName(std::string key) const
{
	auto it = _mapName.find(key);//指定キーを取得
	if (_mapName.end() == it) {//無かったら
		return 0;//存在しない
	}
	else {
		return it->second;//あったら値を返す
	}
}
std::shared_ptr<Sound> SoundFactory::getSoundbyId(int id) const
{
	auto it = _mapId.find(id);//指定キーを取得
	if (_mapId.end() == it) {//無かったら
		return nullptr;//存在しない
	}
	else {
		return it->second;//あったら値を返す
	}
}

int SoundFactory::getSoundNum()const
{
	return 0;
}

void SoundFactory::drawSoundNum()const
{

}
