#include "MediaFoundation.h"

bool MediaFoundation::Init()
{
	CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	HRESULT hr = MFStartup(MF_VERSION);
	if (FAILED(hr))
	{
		MSG(L"Media Foundationの初期化に失敗しました。");
		return false;
	}
	return true;
}

MediaFoundation::~MediaFoundation()
{
	MFShutdown();//MediaFoundationをシャットダウンする
	
}

bool MediaFoundation::LoadSound(const std::wstring path, Sound& sound)
{
	HRESULT hr = S_OK;

	SetDataDirectory();
	IMFSourceReader* pMFSourceReader{ nullptr };
	// ソースリーダーの作成
	hr = MFCreateSourceReaderFromURL(path.c_str(), NULL, &pMFSourceReader);
	if (FAILED(hr))
	{
		WCHAR s[256];
		swprintf(s, 256, L"ファイルの読み込みに失敗しました。%s", path.c_str());
		MSG(s);
		return false;
	}

 	IMFMediaType* pMFMediaType{ nullptr };
	MFCreateMediaType(&pMFMediaType);
	pMFMediaType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
	pMFMediaType->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_PCM);
	pMFSourceReader->SetCurrentMediaType(MF_SOURCE_READER_FIRST_AUDIO_STREAM, nullptr, pMFMediaType);

	pMFMediaType->Release();
	pMFMediaType = nullptr;
	pMFSourceReader->GetCurrentMediaType(MF_SOURCE_READER_FIRST_AUDIO_STREAM, &pMFMediaType);

	WAVEFORMATEX* waveFormat = nullptr;//フォーマットをサウンド内に書き込む
	MFCreateWaveFormatExFromMFMediaType(pMFMediaType, &waveFormat, nullptr);
	sound.getWaveFormat() = *waveFormat;

	CoTaskMemFree(waveFormat);

	while (true)//ファイルの最後まで読み込む
	{
		IMFSample* pMFSample{ nullptr };
		DWORD dwStreamFlags{ 0 };
		pMFSourceReader->ReadSample(MF_SOURCE_READER_FIRST_AUDIO_STREAM, 0, nullptr, &dwStreamFlags, nullptr, &pMFSample);

		if (dwStreamFlags & MF_SOURCE_READERF_ENDOFSTREAM)//最後まで読み込めたら抜ける
		{
			break;
		}

		IMFMediaBuffer* pMFMediaBuffer{ nullptr };
		pMFSample->ConvertToContiguousBuffer(&pMFMediaBuffer);

		BYTE* pBuffer{ nullptr };
		DWORD cbCurrentLength{ 0 };
		pMFMediaBuffer->Lock(&pBuffer, nullptr, &cbCurrentLength);

		std::vector<BYTE>& soundBuffer = sound.getSoundData();//サウンドの波形データの配列を取得
		soundBuffer.resize(soundBuffer.size() + cbCurrentLength);//読み込んだデータ分だけ配列を伸ばす
		memcpy(soundBuffer.data() + soundBuffer.size() - cbCurrentLength, pBuffer, cbCurrentLength);

		pMFMediaBuffer->Unlock();

		pMFMediaBuffer->Release();
		pMFSample->Release();

	}
	pMFSourceReader->Release();

	return true;
}

