#pragma once
#include "Core/Base.h"

//テクスチャクラス：テクスチャの情報を保持するクラス。
class Texture
{

public:
	Texture() {
		_texId = 0;
		m_info = { 0 };
		m_srv = nullptr;
	};
	~Texture() {
		m_info = { 0 };
		m_srv = nullptr;
	};

	int getId()const
	{
		return _texId;
	}
	std::string getTexName()const
	{
		return _texName;
	}

	ID3D11ShaderResourceView** getTexResource()
	{
		return m_srv.GetAddressOf();
	}
	TexMetadata getTexInfo()
	{
		return m_info;
	}

	TexMetadata* getTexInfoPtr()
	{
		return &m_info;
	}
protected:

	friend class TextureFactory;
	
private:

	// 画像情報
	TexMetadata m_info = { 0 };
	//画像ハンドル
	ComPtr<ID3D11ShaderResourceView> m_srv=nullptr;

	int _texId=0;
	std::string _texName = "";

};
