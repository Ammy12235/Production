#pragma once

#include "Core/Singleton.h"


#define MAX_FONT_NUM 30
#define MAX_BITMAP_NUM 100

//DirectWriteクラス：文字の描画をするクラス。
class DirectWrite :public Singleton<DirectWrite>
{
public:

	HRESULT Initialize(IDXGISwapChain* g_pSwapChain,HWND hWnd);
	HRESULT InitBuffer();
	IWICFormatConverter* CreateBitMapDecorder(const std::string& filename);
	int CreateFontHandle(const WCHAR* fontName, float fontSize);
	int CreateD2DBitMap(const std::string& fileName);

	void DrawFormatText(const WCHAR* text, float x, float y, float w, float h, float* color, float alpha, int textHandle);

	void RegisterFontFace(const std::wstring& fontFamilyName, ComPtr<IDWriteFontFile>& fontFile);
	void RegisterTextPathGeometry(const std::string& key, const std::wstring& text, const std::wstring& fontFamilyName, const FLOAT size);
	void DrawTextWithEdge(D2D1_POINT_2F position, const std::wstring& text, const std::string& textPathGeometryKey,
		const std::string& solidColorBrushKey, const std::string& solidColorBrushKeyOfEdge, const float strokeWidth, const float alpha, const float slideRate)const;

	void LoadFontFile(const WCHAR* fontFileName);
	void DrawD2DBitMap(float x, float y, float w, float h, float alpha, int bitmapHandle);
	void DrawD2DBox(float x, float y, float w, float h, float* color, float alpha, float strokeWidth, bool isFill);
	void CleanupDevice();
	void Draw2DStart();
	void Draw2DEnd();

	DirectWrite() {}
	~DirectWrite()
	{
		CleanupDevice();
	}

private:

	//D2Dファクトリー
	ID2D1Factory* p_d2dFactory = nullptr;
	//DirectWriteファクトリ－
	IDWriteFactory5* p_writeFactory = nullptr;
	//テキストフォーマット
	IDWriteTextFormat* p_textFormat[MAX_FONT_NUM] = { nullptr };
	//2Dレンダーターゲット
	ID2D1RenderTarget* p_d2dRT = nullptr;
	//ソリッドブラシ
	ID2D1SolidColorBrush* p_SolidBrush = nullptr;
	//バッファー
	IDXGISurface* p_textBackBuffer = nullptr;
	//フォントファイル
	IDWriteFontFile* p_fontFile=nullptr;

	//ビットマップデコーダー
	IWICImagingFactory* pWICImagingFactory=nullptr;
	IWICBitmapDecoder* pWICBitmapDecoder=nullptr;
	IWICBitmapFrameDecode* pWICBitmapFrameDecode=nullptr;
	//WICフォーマットコンバーター
	IWICFormatConverter* pWICFormatConverter=nullptr;
	//Direct2Dビットマップ画像
	ID2D1Bitmap* pD2DBitmap[MAX_BITMAP_NUM];

	//Datファイルからの読み込みに必要
	//フォントファイルローダー
	IDWriteInMemoryFontFileLoader* p_memoryFontFileLoader = nullptr;
	//フォントセット
	IDWriteFontSet* p_fontSet = nullptr;
	IDWriteFontCollection1* p_fontCollection1 = nullptr;

	std::map<std::string, ComPtr<IDWriteFontFile>& > fontFaceMap;
	std::map<std::string, ComPtr<ID2D1GeometrySink>& > pathGeometryMap;

	int bitmapIndex = -1;
	int fontIIndex = -1;



};

#define DWRITE DirectWrite::GetInstance()
