#pragma once

#include "Base.h"

#define MAX_IMAGE 128

//テクスチャクラス
class Texture
{
public:
	Texture() {};
	~Texture();
	// 画像情報
	TexMetadata m_info = { 0 };
	//画像ハンドル
	ComPtr<ID3D11ShaderResourceView> m_srv;
private:
};

//テクスチャーファクトリークラス：テクスチャの生成をする。また、生成の重複を避ける役割を持つ
class TextureFactory
{
public:

	void RemoveDevice();

	// 画像ファイルを読み込む
	bool  LoadProt(const std::string& filename);
	HRESULT Load(const std::string& filename);
	HRESULT LoadDiv(const std::string& filename, int AllNum, int XNum, int YNum, int XSize, int YSize, int* imageHandle);


	Texture* getTexture(std::string key) const;

private:

	TextureFactory(const  TextureFactory& r) = default;
	TextureFactory& operator=(const  TextureFactory & r) = default;

	static inline TextureFactory* s_instance;

public:

	TextureFactory();
	virtual ~TextureFactory();
	void setTexture(std::string key, Texture* texture);
	std::vector<std::shared_ptr<Texture>> _tex;
	std::unordered_map<std::string, Texture*> _map;//テクスチャをマップで管理する

	static void CreateInstance() {
		if (!s_instance) {
			s_instance = new TextureFactory;
		}
	}

	static void DeleteInstance() {
		delete s_instance;
		s_instance = nullptr;
	}
	static TextureFactory& GetInstance() {
		return *s_instance;
	}

};

#define TEX_FAC TextureFactory::GetInstance()