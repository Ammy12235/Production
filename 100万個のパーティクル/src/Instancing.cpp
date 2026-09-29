#include "Instancing.h"
#include "DEFINE.h"

//シェーダファイルをコンパイルする
HRESULT Instancing::CompileShaderFromFile(const WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut)
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

// リソースの初期化
HRESULT Instancing::Init(ID3D11Device* pD3DDevice, ID3D11DeviceContext* pD3DDeviceContext)
{
	HRESULT hr = E_FAIL;
	m_pDevice = pD3DDevice;
	m_pDeviceContext = pD3DDeviceContext;
	ID3D10Blob* pVSBlob = NULL, * pPSBlob = NULL;
	SetShaderDirectory();
	// 頂点シェーダーを作成する
	// バーテックスシェーダのコンパイル
	hr = CompileShaderFromFile(hlslSrc, vs_main, "vs_5_0", &pVSBlob);
	if (FAILED(hr))
	{
		MessageBox(nullptr,
			L"HLSLファイルをコンパイルできません。HLSLファイルを含むディレクトリからこの実行可能ファイルを実行してください。", L"Error", MB_OK);
		return hr;
	}

	// バーテックスシェーダの作成
	hr = pD3DDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &m_pVertexShader);
	if (FAILED(hr))
	{

		pVSBlob->Release();
		return hr;

	}
	D3D11_INPUT_ELEMENT_DESC layout[] = {
		  { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0,  0, D3D11_INPUT_PER_VERTEX_DATA,   0 },
		  { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA,   0 },
		  // 入力アセンブラにジオメトリ処理用の行列を追加設定する
		  { "INSTANCE_POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    1,  0, D3D11_INPUT_PER_INSTANCE_DATA,   1 },
		  { "INSTANCE_TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
	};
	UINT numElements = ARRAYSIZE(layout);

	// インプットレイアウトの作成
	hr = pD3DDevice->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(),
		pVSBlob->GetBufferSize(), &m_pLayout);
	if (FAILED(hr))
		return hr;

	// ピクセルシェーダのコンパイル
	hr = CompileShaderFromFile(hlslSrc, ps_main_01, "ps_5_0", &pPSBlob);
	if (FAILED(hr))
	{
		MessageBox(nullptr,
			L"HLSLファイルをコンパイルできません。HLSLファイルを含むディレクトリからこの実行可能ファイルを実行してください。", L"Error", MB_OK);
		return hr;
	}

	// ピクセルシェーダの作成
	hr = pD3DDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &m_pPixelShader);
	if (FAILED(hr))
	{

		return hr;
	}

	// *****************************************************************************************************************
	// 頂点バッファを作成
	// *****************************************************************************************************************

	Vertex2D v[] = {
			{{0, size , 0}, {0, 1}},	// 左下
			{{0 , 0 , 0}, {0, 0}},	// 左上
			{{size , size , 0}, {0.25f, 1}},	// 右下
			{{size , 0 , 0},{0.25f, 0}},	// 右上
	};

	//頂点バッファ
	D3D11_BUFFER_DESC vbDesc = {};
	vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;	// デバイスにバインドするときの種類(頂点バッファ、インデックスバッファ、定数バッファなど)
	vbDesc.ByteWidth = sizeof(Vertex2D) * 4;	// 作成するバッファのバイトサイズ
	vbDesc.MiscFlags = 0;							// その他のフラグ
	vbDesc.StructureByteStride = 0;					// 構造化バッファの場合、その構造体のサイズ

	vbDesc.Usage = D3D11_USAGE_DYNAMIC;				// 作成するバッファの使用法
	vbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	D3D11_SUBRESOURCE_DATA resource;
	resource.pSysMem = &v;
	resource.SysMemPitch = 0;
	resource.SysMemSlicePitch = 0;
	hr = pD3DDevice->CreateBuffer(&vbDesc, &resource, &m_pVertexBuffer);


	//インスタンスバッファ
	vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;	// デバイスにバインドするときの種類(頂点バッファ、インデックスバッファ、定数バッファなど)
	vbDesc.ByteWidth = sizeof(InstanceData) * maxInstanceCount;	// 作成するバッファのバイトサイズ
	vbDesc.MiscFlags = 0;							// その他のフラグ
	vbDesc.StructureByteStride = 0;					// 構造化バッファの場合、その構造体のサイズ

	vbDesc.Usage = D3D11_USAGE_DYNAMIC;				// 作成するバッファの使用法
	vbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	hr = pD3DDevice->CreateBuffer(&vbDesc, nullptr, &m_pInstanceBuffer);
	// 
	// *****************************************************************************************************************
	// 定数バッファを作成
	// *****************************************************************************************************************

	D3D11_BUFFER_DESC BufferDesc;
	//頂点用
	::ZeroMemory(&BufferDesc, sizeof(BufferDesc));
	BufferDesc.ByteWidth = sizeof(CBUFFER0);        // バッファサイズ
	BufferDesc.Usage = D3D11_USAGE_DYNAMIC;       // リソース使用法を特定する
	BufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;// バッファの種類
	BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;    // CPU アクセス
	BufferDesc.MiscFlags = 0;                         // その他のフラグも設定しない
	hr = pD3DDevice->CreateBuffer(&BufferDesc, NULL, &m_pConstantBuffer);
	if (FAILED(hr)) return hr;

	hr = S_OK;
	for (int y = 0; y < instSize; y++)
	{
		for (int x = 0; x < instSize; x++)
		{
			int index = y * instSize + x;
			instances[index].pos = XMFLOAT3(x * size, y * size, 0);



			std::random_device rd;
			if(rd()%4==0)instances[index].uvOffset = XMFLOAT2(0, 0);
			else if (rd() % 4 == 1)instances[index].uvOffset = XMFLOAT2(0.25f, 0);
			else if (rd() % 4 == 2)instances[index].uvOffset = XMFLOAT2(0.5f, 0);
			else if (rd() % 4 == 3)instances[index].uvOffset = XMFLOAT2(0.75, 0);



		}
	}
EXIT:
	return hr;
}

// メモリ開放
void Instancing::CleanupDevice()
{
	SAFE_RELEASE(m_pLayout);
	SAFE_RELEASE(m_pConstantBuffer);
	SAFE_RELEASE(m_pVertexBuffer);
	SAFE_RELEASE(m_pInstanceBuffer);
	SAFE_RELEASE(m_pVertexShader);
	SAFE_RELEASE(m_pPixelShader);
}
XMFLOAT2 getUvOffset(int tx, int ty, int tipSize, int texSizeX, int texSizeY)
{
	float u = (float)tipSize / texSizeX;
	float v = (float)tipSize / texSizeY;
	return XMFLOAT2(tx * u, ty * v);
}

float instTimer = 2.0f;
// 描画処理
void Instancing::DrawInstancedBox(const Texture* tex, int xCount, int yCount, int mapSize)
{
	if (yCount * xCount > maxInstanceCount)return;
	instTimer += 0.0016f;
	for (int y = 0; y < yCount; y++)
	{
		for (int x = 0; x < xCount; x++)
		{
			int index = y * xCount + x;
			instances[index].pos = XMFLOAT3(x * mapSize+cos(instTimer+y)*128, y * mapSize + sin(instTimer + x)*128, 0);
		}
	}

	instTimer += 0.016f;
	UINT strides[2] = { sizeof(Vertex2D),sizeof(InstanceData) };
	UINT offsets[2] = { 0,0 };
	ID3D11Buffer* buffers[2] = { m_pVertexBuffer, m_pInstanceBuffer };

	m_pDeviceContext->VSSetShader(m_pVertexShader, nullptr, 0);
	m_pDeviceContext->PSSetShader(m_pPixelShader, nullptr, 0);
	// インプットレイアウトの設定
	m_pDeviceContext->IASetInputLayout(m_pLayout);

	m_pDeviceContext->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);
	m_pDeviceContext->GSSetConstantBuffers(0, 1, &m_pConstantBuffer);
	m_pDeviceContext->PSSetConstantBuffers(0, 1, &m_pConstantBuffer);
	//頂点バッファとインスタンスバッファをバインド
	m_pDeviceContext->IASetVertexBuffers(0, 2, buffers, strides, offsets);

	D3D11_MAPPED_SUBRESOURCE pData;

	// 頂点バッファにデータを書き込む(インスタンスバッファ)
	if (SUCCEEDED(m_pDeviceContext->Map(m_pInstanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &pData)))
	{
		// データコピー
		memcpy_s(pData.pData, sizeof(InstanceData) * maxInstanceCount, &instances[0], sizeof(InstanceData) * maxInstanceCount);

		m_pDeviceContext->Unmap(m_pInstanceBuffer, 0);
	}
	g_World = XMMatrixIdentity();//ワールド座標系初期化
	//定数バッファにデータを書き込む
	CBUFFER0 cb;
	if (SUCCEEDED(m_pDeviceContext->Map(m_pConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &pData)))
	{
		cb.matWVP = XMMatrixTranspose(g_World);
		cb.viewPortWidth = Define::WIN_W;
		cb.viewPortHeight = Define::WIN_H;
		cb.alpha = alpha;
		memcpy_s(pData.pData, pData.RowPitch, (void*)(&cb), sizeof(cb));
		m_pDeviceContext->Unmap(m_pConstantBuffer, 0);
	}
	// テクスチャを、スロット0にセット
	m_pDeviceContext->PSSetShaderResources(0, 1, tex->m_srv.GetAddressOf());
	m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	m_pDeviceContext->DrawInstanced(4, maxInstanceCount, 0, 0);
}
