#include "ShaderUtility.h"

//シェーダファイルをコンパイルする
HRESULT ShaderUtility::CompileShaderFromFile(const WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut)
{
	HRESULT hr = S_OK;

	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
	dwShaderFlags |= D3DCOMPILE_DEBUG;

	dwShaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	ID3DBlob* pErrorBlob = nullptr;
	hr = D3DCompileFromFile(szFileName, nullptr, nullptr, szEntryPoint, szShaderModel,
		dwShaderFlags, 0, ppBlobOut, &pErrorBlob);
	if (FAILED(hr))
	{
		if (pErrorBlob)
		{
			OutputDebugStringA(reinterpret_cast<const char*>(pErrorBlob->GetBufferPointer()));
			pErrorBlob->Release();
		}
		return hr;
	}
	if (pErrorBlob) pErrorBlob->Release();

	return S_OK;
}


//RTVを作成する
HRESULT ShaderUtility::CreateRenderTargetView(ID3D11Device* pD3DDevice, ID3D11RenderTargetView** ppRTView, UINT Width, UINT Height, DXGI_FORMAT Format) const
{
	HRESULT hr = E_FAIL;

	ID3D11Texture2D* pTexture2D = nullptr;

	D3D11_TEXTURE2D_DESC Tex2DDesc;
	D3D11_RENDER_TARGET_VIEW_DESC RTVDesc;

	if (Width == 0 || Height == 0)
		goto EXIT;

	::ZeroMemory(&Tex2DDesc, sizeof(D3D11_TEXTURE2D_DESC));
	Tex2DDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	Tex2DDesc.Usage = D3D11_USAGE_DEFAULT;
	Tex2DDesc.Format = Format;
	Tex2DDesc.Width = Width;
	Tex2DDesc.Height = Height;
	Tex2DDesc.CPUAccessFlags = 0;
	Tex2DDesc.MipLevels = 1;
	Tex2DDesc.ArraySize = 1;
	Tex2DDesc.SampleDesc.Count = 1;
	Tex2DDesc.SampleDesc.Quality = 0;

	::ZeroMemory(&RTVDesc, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));
	RTVDesc.Format = Tex2DDesc.Format;
	RTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	RTVDesc.Texture2D.MipSlice = 0;

	hr = pD3DDevice->CreateTexture2D(&Tex2DDesc, nullptr, &pTexture2D);
	if (FAILED(hr))
		goto EXIT;

	hr = pD3DDevice->CreateRenderTargetView(pTexture2D, &RTVDesc, ppRTView);
	if (FAILED(hr))
		goto EXIT;

	SAFE_RELEASE(pTexture2D);

	hr = S_OK;
EXIT:
	return hr;
}

// RTVからSRVを作成する
ID3D11ShaderResourceView* ShaderUtility::GetSRViewFromRTView(ID3D11Device* pD3DDevice, ID3D11RenderTargetView* pRTView) const
{
	ID3D11Resource* pResource = NULL;
	ID3D11ShaderResourceView* pSRView = NULL;

	if (pRTView == nullptr)
		return pSRView;

	pRTView->GetResource(&pResource);
	pD3DDevice->CreateShaderResourceView(pResource, NULL, &pSRView);
	SAFE_RELEASE(pResource);

	return pSRView;
}

ID3D11ShaderResourceView* ShaderUtility::GetSRViewFromTexture(ID3D11Device* pD3DDevice, ID3D11Texture2D* pTex2D) const
{
	ID3D11ShaderResourceView* pSRView = NULL;

	if (pTex2D == nullptr)
		return pSRView;
	pD3DDevice->CreateShaderResourceView(pTex2D, NULL, &pSRView);

	return pSRView;
}

// レンダーターゲットビューの縦横のサイズを取得
XMFLOAT2 ShaderUtility::GetRTViewSize(ID3D11RenderTargetView* pRTView) const
{
	XMFLOAT2 Size = XMFLOAT2(0, 0);

	if (pRTView == nullptr)
		return Size;

	ID3D11Resource* pResource = nullptr;
	pRTView->GetResource(&pResource);
	ID3D11Texture2D* pTexture = reinterpret_cast<ID3D11Texture2D*>(pResource);
	D3D11_TEXTURE2D_DESC Desc;
	pTexture->GetDesc(&Desc);
	Size = XMFLOAT2((float)Desc.Width, (float)Desc.Height);
	SAFE_RELEASE(pResource);
	return Size;
}

// シェーダーリソースビューの縦横のサイズを取得
XMFLOAT2 ShaderUtility::GetSRViewSize(ID3D11ShaderResourceView* pSRView) const
{
	XMFLOAT2 Size = XMFLOAT2(0, 0);

	if (pSRView == nullptr)
		return Size;

	ID3D11Resource* pResource = nullptr;
	pSRView->GetResource(&pResource);
	ID3D11Texture2D* pTexture = reinterpret_cast<ID3D11Texture2D*>(pResource);
	D3D11_TEXTURE2D_DESC Desc;
	pTexture->GetDesc(&Desc);
	Size = XMFLOAT2((float)Desc.Width, (float)Desc.Height);
	SAFE_RELEASE(pResource);
	return Size;
}
// 頂点、インデックス、定数バッファを作成する
HRESULT ShaderUtility::CreateBuffer(ID3D11Device* pD3DDevice, ID3D11Buffer** pBuffer, void* pData, size_t size, UINT CPUAccessFlag, D3D11_BIND_FLAG BindFlag)
{
	HRESULT hr = E_FAIL;

	// バッファー リソース。
	// D3D11_BUFFER_DESC
	D3D11_BUFFER_DESC BufferDesc;

	// サブリソース
	// D3D11_SUBRESOURCE_DATA
	D3D11_SUBRESOURCE_DATA* resource = NULL;

	D3D11_USAGE Usage = D3D11_USAGE_DEFAULT;
	UINT CPUAccessFlags = 0;

	switch (CPUAccessFlag)
	{
		// CPUアクセスを許可しない
	case 0:
		Usage = D3D11_USAGE_DEFAULT;
		CPUAccessFlags = CPUAccessFlag;
		break;
		// CPUアクセスを許可する
	default:
		Usage = D3D11_USAGE_DYNAMIC;
		CPUAccessFlags = CPUAccessFlag;
		break;
	}

	// 初期値を設定する
	if (pData)
	{
		resource = new D3D11_SUBRESOURCE_DATA();
		resource->pSysMem = pData;
		resource->SysMemPitch = 0;
		resource->SysMemSlicePitch = 0;
	}

	// バッファの設定
	::ZeroMemory(&BufferDesc, sizeof(BufferDesc));
	BufferDesc.ByteWidth = size;                      // バッファサイズ
	BufferDesc.Usage = Usage;                     // リソース使用法を特定する
	BufferDesc.BindFlags = BindFlag;                  // バッファの種類
	BufferDesc.CPUAccessFlags = CPUAccessFlags;            // CPU アクセス
	BufferDesc.MiscFlags = 0;                         // その他のフラグも設定しない

	// バッファを作成する
	hr = pD3DDevice->CreateBuffer(&BufferDesc, resource, pBuffer);
	if (FAILED(hr)) goto EXIT;

	hr = S_OK;

EXIT:
	SAFE_DELETE(resource);
	return hr;
}

// 頂点バッファを作成する
HRESULT ShaderUtility::CreateVertexBuffer(ID3D11Device* pD3DDevice, ID3D11Buffer** pBuffer, void* pData, size_t size, UINT CPUAccessFlag)
{
	return CreateBuffer(pD3DDevice,pBuffer, pData, size, CPUAccessFlag, D3D11_BIND_VERTEX_BUFFER);
}

// インデックスバッファを作成する
HRESULT ShaderUtility::CreateIndexBuffer(ID3D11Device* pD3DDevice, ID3D11Buffer** pBuffer, void* pData, size_t size, UINT CPUAccessFlag)
{
	return CreateBuffer(pD3DDevice, pBuffer, pData, size, CPUAccessFlag, D3D11_BIND_INDEX_BUFFER);
}

// 定数バッファを作成する
HRESULT ShaderUtility::CreateConstantBuffer(ID3D11Device* pD3DDevice, ID3D11Buffer** pBuffer, void* pData, size_t size, UINT CPUAccessFlag)
{
	return CreateBuffer(pD3DDevice, pBuffer, pData, size, CPUAccessFlag, D3D11_BIND_CONSTANT_BUFFER);
}
