#include "DIRECTWRITE.h"

HRESULT DirectWrite::Initialize(IDXGISwapChain* g_pSwapChain)
{
	HRESULT hr;
	Log("	DirecWriteの初期化を開始\n");
	// Direct2D,DirectWriteの初期化
	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &p_d2dFactory);
	if (FAILED(hr))
		return hr;

	hr = g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&p_textBackBuffer));
	if (FAILED(hr))
		return hr;

	FLOAT dpiX;
	FLOAT dpiY;
	p_d2dFactory->GetDesktopDpi(&dpiX, &dpiY);

	D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(D2D1_RENDER_TARGET_TYPE_DEFAULT, D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED), dpiX, dpiY);

	hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(&p_writeFactory));
	if (FAILED(hr))
		return hr;

	DirectWrite::InitBuffer();
	Log("	DirecWriteの初期化完了\n");
	return S_OK;
}

/*
void DirectWrite::LoadFontFile(const WCHAR* fontFileName)
{
	//フォントファイルを開く
	FILE* fp;
	errno_t err;
	fopen_s(&fp, (char*)fontFileName, "r");

	//ファイルサイズの取得
	struct stat statBuf;
	stat((char*)fontFileName, &statBuf);

	//フォントセットビルダー
	IDWriteFontSetBuilder1* p_fontSetBuilder;
	p_writeFactory->CreateFontSetBuilder(&p_fontSetBuilder);
	p_writeFactory->CreateFontFileReference(fontFileName, nullptr, &p_fontFile);

	p_writeFactory->CreateInMemoryFontFileLoader(&p_memoryFontFileLoader);
	p_writeFactory->RegisterFontFileLoader(p_memoryFontFileLoader);

	p_memoryFontFileLoader->CreateInMemoryFontFileReference(p_writeFactory, fp, statBuf.st_size, nullptr, &p_fontFile);

	//フォントコレクションを生成
	p_fontSetBuilder->AddFontFile(p_fontFile);
	p_fontSetBuilder->CreateFontSet(&p_fontSet);
	p_writeFactory->CreateFontCollectionFromFontSet(p_fontSet, &p_fontCollection1);
	//フォントフェイスを登録
	RegisterFontFace(fontFileName, p_fontFile);

	fclose(fp);
}
*/


int DirectWrite::CreateFontHandle(const WCHAR* fontName, float fontSize)
{
	HRESULT hr;
	if (fontIIndex > MAX_FONT_NUM)return -1;
	fontIIndex++;
	//関数CreateTextFormat()
	//第1引数：フォント名（L"メイリオ", L"Arial", L"Meiryo UI"等）
	//第2引数：フォントコレクション（datファイルから読み込む場合）
	//第3引数：フォントの太さ（DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_WEIGHT_BOLD等）
	//第4引数：フォントスタイル（DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STYLE_OBLIQUE, DWRITE_FONT_STYLE_ITALIC）
	//第5引数：フォントの幅（DWRITE_FONT_STRETCH_NORMAL,DWRITE_FONT_STRETCH_EXTRA_EXPANDED等）
	//第6引数：フォントサイズ（20, 30等）
	//第7引数：ロケール名（L""）
	//第8引数：テキストフォーマット（&g_pTextFormat）
	hr = p_writeFactory->CreateTextFormat(fontName, nullptr/*p_fontCollection1*/, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, fontSize, L"", &p_textFormat[fontIIndex]);
	if (FAILED(hr))
		return -1;

	//関数SetTextAlignment()
	//第1引数：テキストの配置（DWRITE_TEXT_ALIGNMENT_LEADING：前, DWRITE_TEXT_ALIGNMENT_TRAILING：後, DWRITE_TEXT_ALIGNMENT_CENTER：中央,
	//                         DWRITE_TEXT_ALIGNMENT_JUSTIFIED：行いっぱい）
	hr = p_textFormat[fontIIndex]->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
	if (FAILED(hr))
		return -1;
	hr = p_textFormat[fontIIndex]->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
	if (FAILED(hr))
		return -1;

	return fontIIndex;
}

IWICFormatConverter* DirectWrite::CreateBitMapDecorder(const std::string& filename)
{
	// マルチバイト文字列からワイド文字列へ変換
	setlocale(LC_CTYPE, "jpn");
	wchar_t wFilename[256];
	size_t ret;
	mbstowcs_s(&ret, wFilename, filename.c_str(), 256);
	HRESULT hr;
	hr = CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_IWICImagingFactory, (LPVOID*)&pWICImagingFactory);
	if (FAILED(hr))
		return 0;

	//関数CreateDecoderFromFilename()
	//第1引数：ファイル名
	hr = pWICImagingFactory->CreateDecoderFromFilename(wFilename, nullptr, GENERIC_READ, WICDecodeMetadataCacheOnLoad, &pWICBitmapDecoder);
	if (FAILED(hr))
		return 0;

	hr = pWICBitmapDecoder->GetFrame(0, &pWICBitmapFrameDecode);
	if (FAILED(hr))
		return 0;

	hr = pWICImagingFactory->CreateFormatConverter(&pWICFormatConverter);
	if (FAILED(hr))
		return 0;

	hr = pWICFormatConverter->Initialize(pWICBitmapFrameDecode, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, nullptr, 1.0f, WICBitmapPaletteTypeMedianCut);
	if (FAILED(hr))
		return 0;

	return pWICFormatConverter;
}

int DirectWrite::CreateD2DBitMap(const std::string& fileName)
{
	HRESULT hr;
	if (bitmapIndex > MAX_BITMAP_NUM)return -1;
	bitmapIndex++;
	pWICFormatConverter = CreateBitMapDecorder(fileName);
	hr = p_d2dRT->CreateBitmapFromWicBitmap(pWICFormatConverter, nullptr, &pD2DBitmap[bitmapIndex]);
	if (FAILED(hr))
		return -1;
	return bitmapIndex;
}

/*
void DirectWrite::RegisterFontFace(const std::wstring& fontFamilyName, ComPtr<IDWriteFontFile>& fontFile) noexcept {
	std::array<IDWriteFontFile*, 1U> fontFileArray = { fontFile.Get() };
	ComPtr<IDWriteFontFace> fontFace = nullptr;
	if (SUCCEEDED(p_writeFactory->CreateFontFace(DWRITE_FONT_FACE_TYPE_TRUETYPE, 1U, fontFile, 0U, DWRITE_FONT_SIMULATIONS_NONE, fontFace.ReleaseAndGetAddressOf()))) {
		fontFaceMap[fontFamilyName] = fontFace;
	}
}

void DirectWrite::RegisterTextPathGeometry(const std::string& key, const std::wstring& text, const std::wstring& fontFamilyName, const FLOAT size) noexcept {
	//グリフインデックス列を取得する
	std::vector<UINT> codePoints;
	auto glyphIndices = new UINT16[text.length()];
	ZeroMemory(glyphIndices, sizeof(UINT16) * text.length());
	for (auto character : text) {
		codePoints.emplace_back(character);
	}
	const auto fontFace = fontFaceMap[fontFamilyName];
	fontFace->GetGlyphIndicesW(codePoints.data(), static_cast<UINT32>(codePoints.size()), glyphIndices);
	ComPtr<ID2D1PathGeometry> pathGeometry = nullptr;
	if (FAILED(p_d2dFactory->CreatePathGeometry(pathGeometry.ReleaseAndGetAddressOf()))) {
		return;
	}
	ComPtr<ID2D1GeometrySink> geometrySink = nullptr;
	if (FAILED(pathGeometry->Open(geometrySink.ReleaseAndGetAddressOf()))) {
		return;
	}
		//アウトライン情報を取得する
		if (FAILED(fontFace->GetGlyphRunOutline((size / 72.0f) * 96.0f, glyphIndices, nullptr, nullptr, static_cast<UINT32>(text.length()), FALSE, FALSE, geometrySink.Get()))) {
			return;
		}
			if (FAILED(geometrySink->Close())){
				return;
			}
		codePoints.clear();
		delete[] glyphIndices;
		pathGeometryMap[key] = { pathGeometry, geometrySink };
}

void DirectWrite::DrawTextWithEdge(D2D1_POINT_2F position, const std::wstring& text, const std::string& textPathGeometryKey, const std::string& solidColorBrushKey,
	const std::string& solidColorBrushKeyOfEdge, const float strokeWidth, const float alpha, const float slideRate) const noexcept {
	const auto textPathGeometry = pathGeometryMap.at(textPathGeometryKey).first;
	const auto solidColorBrush = solidColorBrushMap.at(solidColorBrushKey);
	const auto solidColorBrushOfEdge = solidColorBrushMap.at(solidColorBrushKeyOfEdge);
	const auto d2dDeviceContext = parent->getD2DDeviceContext();

	if (slideRate > 0.0f) {
		D2D1_RECT_F rect{};
		textPathGeometry->GetBounds(nullptr, &rect);
		position.x -= (rect.right - rect.left) * slideRate;
	}
	solidColorBrush->SetOpacity(alpha);
	solidColorBrushOfEdge->SetOpacity(alpha);
	d2dDeviceContext->SetTransform(D2D1::Matrix3x2F::Translation(position.x, position.y));
	d2dDeviceContext->DrawGeometry(textPathGeometry.Get(), solidColorBrushOfEdge.Get(), strokeWidth);
	d2dDeviceContext->FillGeometry(textPathGeometry.Get(), solidColorBrush.Get());
	solidColorBrush->SetOpacity(1.0f);
	solidColorBrushOfEdge->SetOpacity(1.0f);
}
*/
HRESULT DirectWrite::InitBuffer()
{
	HRESULT hr;

	D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(D2D1_RENDER_TARGET_TYPE_DEFAULT, D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED), 0, 0);
	hr = p_d2dFactory->CreateDxgiSurfaceRenderTarget(p_textBackBuffer, &props, &p_d2dRT);
	if (FAILED(hr))
		return hr;
	//関数CreateSolidColorBrush()
   //第1引数：フォント色（D2D1::ColorF(D2D1::ColorF::Black)：黒, D2D1::ColorF(D2D1::ColorF(0.0f, 0.2f, 0.9f, 1.0f))：RGBA指定）
	hr = p_d2dRT->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &p_SolidBrush);
	if (FAILED(hr))
		return hr;

	return S_OK;
}
//D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBORなどを指定します
void DirectWrite::DrawD2DBitMap(float x, float y, float w, float h, float alpha, int bitmapHandle)
{
	if (bitmapHandle == -1)return;
	p_d2dRT->DrawBitmap(pD2DBitmap[bitmapHandle], D2D1::RectF(x, y, x + w, y + h), alpha, D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR, nullptr);

}
void DirectWrite::DrawFormatText(const WCHAR* text, float x, float y, float w, float h, float* color, float alpha, int fontHandle)
{
	if (fontHandle == -1)return;
	p_SolidBrush->SetColor(D2D1::ColorF(D2D1::ColorF(color[0], color[1], color[2], alpha)));
	int i = 0;
	const WCHAR* pText = text;
	for (; *text; text++)i++;

	p_d2dRT->DrawText(pText, i, p_textFormat[fontHandle], D2D1::RectF(x, y, x + w, y + h), p_SolidBrush, D2D1_DRAW_TEXT_OPTIONS_NONE);
}

void DirectWrite::Draw2DStart()
{
	p_d2dRT->BeginDraw();
}

void DirectWrite::Draw2DEnd()
{
	p_d2dRT->EndDraw();
}

void DirectWrite::CleanupDevice()
{
	p_writeFactory->UnregisterFontFileLoader(p_memoryFontFileLoader);
	for (int i = 0; i < MAX_BITMAP_NUM; i++)
	SAFE_RELEASE(pD2DBitmap[i]);
	SAFE_RELEASE(pWICFormatConverter);
	SAFE_RELEASE(pWICBitmapFrameDecode);
	SAFE_RELEASE(pWICBitmapDecoder);
	SAFE_RELEASE(pWICImagingFactory);


	SAFE_RELEASE(p_memoryFontFileLoader);
	SAFE_RELEASE(p_fontSet);
	SAFE_RELEASE(p_fontCollection1);
	SAFE_RELEASE(p_fontFile);


	SAFE_RELEASE(p_textBackBuffer);
	SAFE_RELEASE(p_SolidBrush);
	SAFE_RELEASE(p_d2dRT);
	for (int i = 0; i < MAX_FONT_NUM; i++)
		SAFE_RELEASE(p_textFormat[i]);
	SAFE_RELEASE(p_writeFactory)
		SAFE_RELEASE(p_d2dFactory);

}