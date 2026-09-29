#include "TextureFactory.h"
#include "Core/Algorithm/Hash.h"
#include "COre/DEFINE.h"
#include "Graphics/Renderer/DirectX11/DIRECT3D11.h"

TextureFactory::TextureFactory()//ループの最初でロードは済ます
{
	_mapName.clear();
	_mapId.clear();
	_tex.reserve(10000);
}

TextureFactory::~TextureFactory()
{
	RemoveDevice();
}
void TextureFactory::RemoveDevice()
{

}
int TextureFactory::CreateTexture(const std::string& filename)
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
	std::shared_ptr<Texture> texture = std::make_shared<Texture>();
	std::unique_ptr<ScratchImage> image = std::make_unique<ScratchImage>();
	
	HRESULT hr = E_FAIL;
	//DDSファイルの読み込み
	if (strcmp(pExtension, "dds") == 0)
	{
		hr = LoadFromDDSFile(path.c_str(), DDS_FLAGS::DDS_FLAGS_NONE, texture->getTexInfoPtr(), *image);
		if(FAILED(hr))
		{
		
			return 0;
		}
	}
	// TGAファイルの読み込み
	else if (strcmp(pExtension, "tga") == 0)
	{
		hr = LoadFromTGAFile(path.c_str(), texture->getTexInfoPtr(), *image);
		if (FAILED(hr))
		{
			
			return 0;
		}
	}
	// WIC画像の読み込み
	else
	{
		hr = LoadFromWICFile(path.c_str(), DirectX::WIC_FLAGS_NONE, texture->getTexInfoPtr(), *image);
		if (FAILED(hr))
		{
			
			return 0;
		}
	}

	// ミップマップの生成
	if (texture->getTexInfo().mipLevels == 1)
	{
		std::unique_ptr<ScratchImage> mipChain = std::make_unique<ScratchImage>();
		if (SUCCEEDED(GenerateMipMaps(image->GetImages(), image->GetImageCount(), image->GetMetadata(), DirectX::TEX_FILTER_DEFAULT, 0, *mipChain)))
		{
			image = std::move(mipChain);
		}
	}
	ID3D11Device* device = D3D.GetDevice();
	// リソースとシェーダーリソースビューを作成
	if (FAILED(CreateShaderResourceView(device, image->GetImages(), image->GetImageCount(), texture->getTexInfo(), texture->getTexResource())))
	{
		
		return 0;
	}

	_tex.push_back(texture);
	Hash hash(filename.c_str());
	int texId = hash.GetDigest();
	setTexture(filename, texture, texId);

	// 成功！
	return texId;
}

//テクスチャを削除する
bool TextureFactory::DeleteTexture(int ID)
{

	for (auto it = _tex.begin(); it != _tex.end();) {
		if ((*it)->getId() == ID) {
			auto texName = (*it)->getTexName();
			_mapId.erase(ID);
			_mapName.erase(texName);
			_tex.erase(it);//テクスチャを削除
			return true;
		}
		else {
			it++;
		}
	}
	return false;//該当するテクスチャは存在しなかった
}

//作成したテクスチャをマップにセットする
void TextureFactory::setTexture(std::string key, std::shared_ptr<Texture> texture, int texId)
{

	//管理側の登録
	_mapName[key] = texId;
	_mapId[texId] = texture;

	//テクスチャ側の登録
	texture->_texId = texId;
	texture->_texName = key;


}

//テクスチャのIDを名前から取得
int TextureFactory::getIDbyName(std::string key)const
{
	auto it = _mapName.find(key);//指定キーを取得
	if (_mapName.end() == it) {//無かったら
		return 0;//存在しない
	}
	else {
		return it->second;//あったら値を返す
	}
}
//テクスチャのポインターをIDから取得
std::shared_ptr<Texture> TextureFactory::getTexturebyId(int id) const
{
	auto it = _mapId.find(id);//指定キーを取得
	if (_mapId.end() == it) {//無かったら
		return nullptr;//存在しない
	}
	else {
		return it->second;//あったら値を返す
	}
}

//テクスチャの総数を返す
int TextureFactory::getTexNum()const
{
	return _tex.size();
}

//テクスチャの総数を表示する
void TextureFactory::drawTexNum()const
{
	if (Define::Debug)
	{
		WCHAR str[128];
		swprintf(str, 128, L"TexNum:%d", _tex.size());
		DWRITE.DrawFormatText(str, 0, 400, 100, 100, D3D.GetColor(255, 255, 255), 1, 1);
	}
}

/*
HRESULT TextureFactory::LoadProt(const std::string& filename)//画像のロード
{
	SetDataDirectory();
	// マルチバイト文字列からワイド文字列へ変換
	setlocale(LC_CTYPE, "jpn");
	wchar_t wFilename[256];
	size_t ret;
	mbstowcs_s(&ret, wFilename, filename.c_str(), 256);

	auto it = _map.find(filename);//指定キーを取得
	if (_map.end() != it)//すでに生成されていたら
		return S_OK;//生成しない

	_tex.push_back(std::make_shared<Texture>());

	HRESULT hr;
	Microsoft::WRL::ComPtr<IWICImagingFactory> WICImagingFactory;
	hr = CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_IWICImagingFactory, (LPVOID*)(&WICImagingFactory));
	if (FAILED(hr))
		return hr;

	Microsoft::WRL::ComPtr<IWICBitmapDecoder> WICBitmapDecoder;
	//関数CreateDecoderFromFilename()
	//第1引数：ファイル名
	hr = WICImagingFactory->CreateDecoderFromFilename(wFilename, nullptr, GENERIC_READ, WICDecodeMetadataCacheOnDemand, &WICBitmapDecoder);
	if (FAILED(hr))
		return hr;

	Microsoft::WRL::ComPtr<IWICBitmapFrameDecode> WICBitmapFrameDecode;
	hr = WICBitmapDecoder->GetFrame(0, &WICBitmapFrameDecode);
	if (FAILED(hr))
		return hr;

	Microsoft::WRL::ComPtr<IWICFormatConverter> WICFormatConverter;
	hr = WICImagingFactory->CreateFormatConverter(&WICFormatConverter);
	if (FAILED(hr))
		return hr;

	hr = WICFormatConverter->Initialize(WICBitmapFrameDecode.Get(), GUID_WICPixelFormat32bppRGBA, WICBitmapDitherTypeNone, nullptr, 1.0f, WICBitmapPaletteTypeMedianCut);
	if (FAILED(hr))
		return hr;

	//テクスチャのサイズを取得
	UINT uiImageWidth = 0;
	UINT uiImageHeight = 0;
	hr = WICFormatConverter->GetSize(&uiImageWidth, &uiImageHeight);
	if (FAILED(hr))
		return hr;
	_tex.back()->m_info.width = uiImageWidth;
	_tex.back()->m_info.height = uiImageHeight;
	//テクスチャの作成
	ComPtr<ID3D11Texture2D> D3DTexture;
	D3D11_TEXTURE2D_DESC td;
	td.Width = uiImageWidth;
	td.Height = uiImageHeight;
	td.MipLevels = 1;
	td.ArraySize = 1;
	td.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	td.SampleDesc.Count = 1;
	td.SampleDesc.Quality = 0;
	td.Usage = D3D11_USAGE_DYNAMIC;
	td.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	td.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	td.MiscFlags = 0;
	hr = D3D.GetDevice()->CreateTexture2D(&td, nullptr, &D3DTexture);
	if (FAILED(hr))
		return hr;

	D3D11_MAPPED_SUBRESOURCE msr;
	D3D.GetDeviceContext()->Map(D3DTexture.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
	WICFormatConverter->CopyPixels(nullptr, uiImageWidth * 4, uiImageWidth * uiImageHeight * 4, (BYTE*)msr.pData);
	D3D.GetDeviceContext()->Unmap(D3DTexture.Get(), 0);

	//シェーダリソースビューの作成
	D3D11_SHADER_RESOURCE_VIEW_DESC srv = {};
	srv.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srv.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srv.Texture2D.MipLevels = 1;
	hr = D3D.GetDevice()->CreateShaderResourceView(D3DTexture.Get(), &srv, _tex.back()->m_srv.GetAddressOf());
	if (FAILED(hr))
		return hr;

	setTexture(filename, _tex.back().get());
	return S_OK;
}
*/

/*
HRESULT TextureFactory::LoadDiv(const std::string& filename, int AllNum, int XNum, int YNum, int XSize, int YSize, int* imageHandle)//分割してロード
{
	SetDataDirectory();

	HRESULT hr;


	//テクスチャのサイズを取得
	UINT uiImageWidth = 0;
	UINT uiImageHeight = 0;
	hr = pWICFormatConverter->GetSize(&uiImageWidth, &uiImageHeight);
	if (FAILED(hr))
		return hr;
	//テクスチャの作成
	ComPtr<ID3D11Texture2D> D3DTexture[10];

	for (int x = 0; x < XNum; x++)
	{
		for (int y = 0; y < YNum; y++)
		{
			D3D11_TEXTURE2D_DESC td;
			td.Width = x*XSize;
			td.Height = uiImageHeight;
			td.MipLevels = 1;
			td.ArraySize = 1;
			td.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			td.SampleDesc.Count = 1;
			td.SampleDesc.Quality = 0;
			td.Usage = D3D11_USAGE_DYNAMIC;
			td.BindFlags = D3D11_BIND_SHADER_RESOURCE;
			td.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			td.MiscFlags = 0;
			hr = DX11.g_pd3dDevice->CreateTexture2D(&td, nullptr, &D3DTexture);
			if (FAILED(hr))
				return hr;
		}
	}


	D3D11_MAPPED_SUBRESOURCE msr;
	DX11.g_pImmediateContext->Map(D3DTexture.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
	WICFormatConverter->CopyPixels(nullptr, uiImageWidth * 4, uiImageWidth * uiImageHeight * 4, (BYTE*)msr.pData);
	DX11.g_pImmediateContext->Unmap(D3DTexture.Get(), 0);

	//シェーダリソースビューの作成
	D3D11_SHADER_RESOURCE_VIEW_DESC srv = {};
	srv.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srv.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srv.Texture2D.MipLevels = 1;
	hr = DX11.g_pd3dDevice->CreateShaderResourceView(D3DTexture.Get(), &srv, &m_srv);
	if (FAILED(hr))
		return hr;


	return S_OK;
}
	*/
