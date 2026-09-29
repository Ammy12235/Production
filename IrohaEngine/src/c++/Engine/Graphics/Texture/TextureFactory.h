#pragma once
#include "Core/Singleton.h"
#include "Texture.h"
class Texture;

//テクスチャーファクトリークラス：テクスチャの生成、削除をするクラス。
class TextureFactory:public Singleton<TextureFactory>
{
public:

	void RemoveDevice();

	// 画像ファイルを読み込む
	int CreateTexture(const std::string& filename);//dds,tga,wicをシェーダーリソースビューにする
	bool DeleteTexture(int ID);//テクスチャを削除する
	
	int getIDbyName(std::string key) const;
	std::shared_ptr<Texture> getTexturebyId(int id) const;
	int getTexNum()const;
	void drawTexNum()const;

	TextureFactory();
	virtual ~TextureFactory();
private:


	void setTexture(std::string key, std::shared_ptr<Texture> texture, int texId);//作成したテクスチャをマップにセットする
	std::vector<std::shared_ptr<Texture>> _tex;
	std::unordered_map<std::string, int> _mapName;//IDと名前を紐づける
	std::unordered_map<int, std::shared_ptr<Texture>> _mapId;//テクスチャをIDで管理する

public:

};

#define TEX_FAC TextureFactory::GetInstance()
