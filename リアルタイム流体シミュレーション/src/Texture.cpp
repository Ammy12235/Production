#include "Base.h"
#include "Texture.h"
#include "DirectX.h"

Texture::~Texture()
{

}

TextureFactory::TextureFactory()//ループの最初でロードは済ます
{

}

TextureFactory::~TextureFactory()
{
	RemoveDevice();
}
void TextureFactory::RemoveDevice()
{

}
bool TextureFactory::LoadProt(const std::string& filename)//プロトタイプ
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

	// WIC画像を読み込む
	auto image = std::make_unique<DirectX::ScratchImage>();
	if (FAILED(LoadFromWICFile(wFilename, DirectX::WIC_FLAGS_NONE, &_tex.back()->m_info, *image)))
	{
		// 失敗
		_tex.back()->m_info = {};
		return false;
	}

	// ミップマップの生成
	if (_tex.back()->m_info.mipLevels == 1)
	{
		auto mipChain = std::make_unique<DirectX::ScratchImage>();
		if (SUCCEEDED(GenerateMipMaps(image->GetImages(), image->GetImageCount(), image->GetMetadata(), DirectX::TEX_FILTER_DEFAULT, 0, *mipChain)))
		{
			image = std::move(mipChain);
		}
	}

	// リソースとシェーダーリソースビューを作成
	if (FAILED(CreateShaderResourceView(D3D.GetDevice(), image->GetImages(), image->GetImageCount(), _tex.back()->m_info, _tex.back()->m_srv.GetAddressOf())))
	{
		// 失敗
		_tex.back()->m_info = {};
		return false;
	}
	setTexture(filename, _tex.back().get());

	// 成功！
	return true;
}

HRESULT TextureFactory::Load(const std::string& filename)//画像のロード
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
//作成したテクスチャをマップにセットする
void TextureFactory::setTexture(std::string key, Texture* texture)
{
	_map[key] = texture;
}

//セットされたテクスチャのポインタを取得
Texture* TextureFactory::getTexture(std::string key)const
{
	auto it = _map.find(key);//指定キーを取得
	if (_map.end() == it) {//無かったら
		return 0;//存在しない
	}
	else {
		return it->second;//あったら値を返す
	}
}

