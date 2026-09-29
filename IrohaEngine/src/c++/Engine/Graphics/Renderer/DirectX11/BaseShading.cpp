#include "BaseShading.h"
#include "Core/Define.h"
#include "Graphics/Shader/ShaderFactory.h"

#define _DEBUG 1

BASE_SHADING::BASE_SHADING()
{
	mVS.clear();
	mPS.clear();
	mGS.clear();
	mHS.clear();
	mDS.clear();
	mCS.clear();
	mLayout.clear();

	mIBuffer.clear();
	mCBuffer.clear();
	mVBuffer.clear();

}

BASE_SHADING::~BASE_SHADING()
{
	CleanupDevice();
	ReleseContainer();
}

void BASE_SHADING::CleanupDevice()
{

}

void BASE_SHADING::ReleseContainer()
{
	for (size_t i = 0; i < mIBuffer.size(); ++i)
	{
		SAFE_RELEASE(mIBuffer[i]);
	}
	for (size_t i = 0; i < mCBuffer.size(); ++i)
	{
		SAFE_RELEASE(mCBuffer[i]);
	}

	for (size_t i = 0; i < mVBuffer.size(); ++i)
	{
		SAFE_RELEASE(mVBuffer[i]);
	}

	for (size_t i = 0; i < mVS.size(); ++i)
	{
		SAFE_RELEASE(mVS[i]);
	}
	for (size_t i = 0; i < mLayout.size(); ++i)
	{
		SAFE_RELEASE(mLayout[i]);
	}
	for (size_t i = 0; i < mPS.size(); ++i)
	{
		SAFE_RELEASE(mPS[i]);
	}
	for (size_t i = 0; i < mGS.size(); ++i)
	{
		SAFE_RELEASE(mGS[i]);
	}
	for (size_t i = 0; i < mHS.size(); ++i)
	{
		SAFE_RELEASE(mHS[i]);
	}
	for (size_t i = 0; i < mDS.size(); ++i)
	{
		SAFE_RELEASE(mDS[i]);
	}
	for (size_t i = 0; i < mCS.size(); ++i)
	{
		SAFE_RELEASE(mCS[i]);
	}

	mIBuffer.clear();
	mCBuffer.clear();
	mVBuffer.clear();

	mVS.clear();
	mLayout.clear();
	mPS.clear();
	mGS.clear();
	mHS.clear();
	mDS.clear();
	mCS.clear();



}

HRESULT BASE_SHADING::Init(ID3D11Device& pD3DDevice, ID3D11DeviceContext& pD3DDeviceContext)
{
	HRESULT hr = E_FAIL;

	Log("	シェーダー初期化開始\n");
	// 行列を列優先で設定し、古い形式の記述を許可しないようにする
	UINT Flag1 = D3D10_SHADER_PACK_MATRIX_COLUMN_MAJOR | D3D10_SHADER_ENABLE_STRICTNESS;
	// 最適化レベルを設定する
#if defined(DEBUG) || defined(_DEBUG)
	Flag1 |= D3D10_SHADER_OPTIMIZATION_LEVEL0;
#else
	Flag1 |= D3D10_SHADER_OPTIMIZATION_LEVEL3;
#endif
	SetShaderDirectory();



	//==================================================================
	// 基本のシェーダーを作成する。2D Instancing2D 3D
	//==================================================================
#if _DEBUG
	ID3D11InputLayout* pLayout = nullptr;
	ID3D11VertexShader* pVertexShader = nullptr;
	ID3D11PixelShader* pPixelShader = nullptr;
	ShaderDesc desc = {};//読み込むシェーダーファイルを定義する

	//=====================================
	// 2D
	//=====================================
	// インプットレイアウトの定義(2D)
	D3D11_INPUT_ELEMENT_DESC layout2D[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXUV",   0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },

	};
	UINT numElement2D = ARRAYSIZE(layout2D);

	//頂点シェーダを読み込む（2個あるのはカメラ行列を考慮するものとしないもので２つ作るため）

	desc.fileName = "BaseShading.hlsl";//シェーダの名前

	//----------------------------------------------------
	// カメラ行列を考慮しない頂点シェーダ
	//----------------------------------------------------
	desc.entryPointName = "VS_Local";//シェーダ内のエントリーポイント名
	desc.shaderModel = "vs_5_0";//シェーダモデルがいくつか
	hr = SHADER_FAC.CreateVertexShader(eGenerateType::FROM_FILE, desc, &pVertexShader, layout2D, numElement2D, &pLayout);
	if (FAILED(hr))
		return hr;
	mVS.emplace_back(pVertexShader);
	mLayout.emplace_back(pLayout);

	//----------------------------------------------------
	// カメラ行列を考慮する頂点シェーダ
	//----------------------------------------------------
	desc.entryPointName = "VS_World";
	desc.shaderModel = "vs_5_0";
	hr = SHADER_FAC.CreateVertexShader(eGenerateType::FROM_FILE, desc, &pVertexShader, layout2D, numElement2D, nullptr);
	if (FAILED(hr))
		return hr;
	mVS.emplace_back(pVertexShader);

	//頂点シェーダを読み込む（2個あるのは単色とテクスチャで２つ作るため）

	//----------------------------------------------------
	// 単色で描画するピクセルシェーダ
	//----------------------------------------------------

	desc.entryPointName = "PS_Raw";
	desc.shaderModel = "ps_5_0";
	hr = SHADER_FAC.CreatePixelShader(eGenerateType::FROM_FILE, desc, &pPixelShader);
	if (FAILED(hr))
		return hr;
	mPS.emplace_back(pPixelShader);

	//----------------------------------------------------
	// テクスチャを貼り付けて描画するピクセルシェーダ
	//----------------------------------------------------

	desc.entryPointName = "PS_Texture";
	desc.shaderModel = "ps_5_0";
	hr = SHADER_FAC.CreatePixelShader(eGenerateType::FROM_FILE, desc, &pPixelShader);
	if (FAILED(hr))
		return hr;
	mPS.emplace_back(pPixelShader);

	//=====================================
	// Instancing2D
	//=====================================
	// インプットレイアウトの定義(Instancing2D)
	D3D11_INPUT_ELEMENT_DESC layoutInstancing2D[] = {
		  { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0,  0, D3D11_INPUT_PER_VERTEX_DATA,   0 },
		  { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA,   0 },
		  // 入力アセンブラにジオメトリ処理用の行列を追加設定する
		  { "INSTANCE_POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    1,  0, D3D11_INPUT_PER_INSTANCE_DATA,   1 },
		  { "INSTANCE_TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
	};
	UINT numElement2DInstancing = ARRAYSIZE(layoutInstancing2D);

	//頂点シェーダーを作成する

	desc.fileName = "Instancing.hlsl";//シェーダの名前

	//----------------------------------------------------
	// GPUインスタンシング頂点シェーダ（uvオフセットが付いており、マップチップ描画などに使う）
	//----------------------------------------------------
	desc.entryPointName = "VS";
	desc.shaderModel = "vs_5_0";

	hr = SHADER_FAC.CreateVertexShader(eGenerateType::FROM_FILE, desc, &pVertexShader, layoutInstancing2D, numElement2DInstancing, &pLayout);
	if (FAILED(hr))
		return hr;
	mVS.emplace_back(pVertexShader);
	mLayout.emplace_back(pLayout);

	//----------------------------------------------------
	// テクスチャを貼り付けて描画するピクセルシェーダ(入力レイアウトがUVのみ)
	//----------------------------------------------------

	desc.entryPointName = "PS";
	desc.shaderModel = "ps_5_0";
	hr = SHADER_FAC.CreatePixelShader(eGenerateType::FROM_FILE, desc, &pPixelShader);
	if (FAILED(hr))
		return hr;
	mPS.emplace_back(pPixelShader);

	//=====================================
	// 3D
	//=====================================
	// インプットレイアウトの定義(3D)
	D3D11_INPUT_ELEMENT_DESC layout3D[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },

	};
	UINT numElement3D = ARRAYSIZE(layout3D);


	desc.fileName = "BaseShading_3D.hlsl";
	desc.entryPointName = "VS";
	desc.shaderModel = "vs_5_0";

	hr = SHADER_FAC.CreateVertexShader(eGenerateType::FROM_FILE, desc, &pVertexShader, layout3D, numElement3D, &pLayout);
	if (FAILED(hr))
		return hr;
	mVS.emplace_back(pVertexShader);
	mLayout.emplace_back(pLayout);

	//ピクセルシェーダーを作成する

	desc.entryPointName = "PS";
	desc.shaderModel = "ps_5_0";

	hr = SHADER_FAC.CreatePixelShader(eGenerateType::FROM_FILE, desc, &pPixelShader);
	if (FAILED(hr))
		return hr;
	mPS.emplace_back(pPixelShader);


#else
	//頂点シェーダーを作成する
	ID3D11VertexShader* pVertexShader;
	ID3D11InputLayout* pLayout;
	//=====================================
	// 2D
	//=====================================
	//VERTEXSHADER_Raw(VS)
	ShaderDesc desc =
	{
		"103",
		 "VS",
		 "vs_5_0"
	};
	hr = SHADER_FAC.CreateVertexShader(eGenerateType::FROM_RESOURCE, desc, &pVertexShader, layout1, numElement1, &pLayout);
	if (FAILED(hr))
		return hr;
	mVS.push_back(pVertexShader);
	mLayout.push_back(pLayout);

	//VERTEXSHADER_2D(VS2)
	desc.fileName = "104";

	hr = SHADER_FAC.CreateVertexShader(eGenerateType::FROM_RESOURCE, desc, &pVertexShader, layout1, numElement1, nullptr);
	if (FAILED(hr))
		return hr;
	mVS.push_back(pVertexShader);

	//ピクセルシェーダーを作成する
	ID3D11PixelShader* pPixelShader;

	//PIXELSHADER_2D_Color(PS)
	desc.fileName = "102";

	hr = SHADER_FAC.CreatePixelShader(eGenerateType::FROM_RESOURCE, desc, &pPixelShader);
	if (FAILED(hr))
		return hr;
	mPS.push_back(pPixelShader);

	//PIXELSHADER_2D_Texture(PS2)
	desc.fileName = "105";

	hr = SHADER_FAC.CreatePixelShader(eGenerateType::FROM_RESOURCE, desc, &pPixelShader);
	if (FAILED(hr))
		return hr;
	mPS.push_back(pPixelShader);

	//=====================================
	// 3D
	//=====================================
	//VERTEXSHADER_3D(VS)
	desc =
	{
		"106",
		 "VS",
		 "vs_5_0"
	};
	hr = SHADER_FAC.CreateVertexShader(eGenerateType::FROM_RESOURCE, desc, &pVertexShader, layout2, numElement2, &pLayout);
	if (FAILED(hr))
		return hr;
	mVS.push_back(pVertexShader);
	mLayout.push_back(pLayout);

	//ピクセルシェーダーを作成する

	//PIXELSHADER_3D_Color(PS)
	desc.fileName = "107";

	hr = SHADER_FAC.CreatePixelShader(eGenerateType::FROM_RESOURCE, desc, &pPixelShader);
	if (FAILED(hr))
		return hr;
	mPS.push_back(pPixelShader);

#endif
	// インプットレイアウトの初期設定
	pD3DDeviceContext.IASetInputLayout(mLayout[LAYOUT_2D]);

	ID3D11Buffer* pBuffer = nullptr;
	// *****************************************************************************************************************
	// 頂点バッファを作成
	// *****************************************************************************************************************
	D3D11_BUFFER_DESC vbDesc = {};

	//=======================================
	//2D用
	//=======================================
	ZeroMemory(&vbDesc, sizeof(vbDesc));
	UINT stride = sizeof(Vertex2D);
	UINT offset = 0;
	vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;	// デバイスにバインドするときの種類(頂点バッファ、インデックスバッファ、定数バッファなど)
	vbDesc.ByteWidth = sizeof(Vertex2D) * 4;	// 作成するバッファのバイトサイズ
	vbDesc.MiscFlags = 0;							// その他のフラグ
	vbDesc.StructureByteStride = 0;					// 構造化バッファの場合、その構造体のサイズ

	vbDesc.Usage = D3D11_USAGE_DYNAMIC;				// 作成するバッファの使用法
	vbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	hr = pD3DDevice.CreateBuffer(&vbDesc, nullptr, &pBuffer);
	if (FAILED(hr))
		return hr;

	mVBuffer.emplace_back(pBuffer);

	//=======================================
	//Instancing2D用
	//=======================================

	//原本を置いておくためのバッファ
	ZeroMemory(&vbDesc, sizeof(vbDesc));
	stride = sizeof(InstanceData2D);
	offset = 0;
	vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;	// デバイスにバインドするときの種類(頂点バッファ、インデックスバッファ、定数バッファなど)
	vbDesc.ByteWidth = sizeof(InstanceData2D) * 4;	// 作成するバッファのバイトサイズ
	vbDesc.MiscFlags = 0;							// その他のフラグ
	vbDesc.StructureByteStride = 0;					// 構造化バッファの場合、その構造体のサイズ

	vbDesc.Usage = D3D11_USAGE_DYNAMIC;				// 作成するバッファの使用法
	vbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	hr = pD3DDevice.CreateBuffer(&vbDesc, nullptr, &pBuffer);
	if (FAILED(hr))
		return hr;

	mVBuffer.emplace_back(pBuffer);

	//どこにどのようにコピーするかを保存するためのバッファ
	vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;	// デバイスにバインドするときの種類(頂点バッファ、インデックスバッファ、定数バッファなど)
	vbDesc.ByteWidth = sizeof(InstanceData2D) * MAX_InstancedIndexNum;	// 作成するバッファのバイトサイズ
	vbDesc.MiscFlags = 0;							// その他のフラグ
	vbDesc.StructureByteStride = 0;					// 構造化バッファの場合、その構造体のサイズ

	vbDesc.Usage = D3D11_USAGE_DYNAMIC;				// 作成するバッファの使用法
	vbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	hr = pD3DDevice.CreateBuffer(&vbDesc, nullptr, &pBuffer);
	if (FAILED(hr))
		return hr;
	mVBuffer.emplace_back(pBuffer);

	//=======================================
	//3D用
	//=======================================
	ZeroMemory(&vbDesc, sizeof(vbDesc));
	vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbDesc.ByteWidth = sizeof(Vertex3D) * 6 * 6;
	vbDesc.MiscFlags = 0;							// その他のフラグ
	vbDesc.StructureByteStride = 0;// 構造化バッファの場合、その構造体のサイズ

	vbDesc.Usage = D3D11_USAGE_DYNAMIC;
	vbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	hr = pD3DDevice.CreateBuffer(&vbDesc, nullptr, &pBuffer);
	if (FAILED(hr))
		return hr;
	mVBuffer.emplace_back(pBuffer);
	// 
	// 頂点バッファを描画で使えるように初期セットする
	pD3DDeviceContext.IASetVertexBuffers(0, 1, &mVBuffer[VERTEX_BUFFER_2D], &stride, &offset);
	// プロミティブ・トポロジーをセット
	pD3DDeviceContext.IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	// *****************************************************************************************************************
	// 定数バッファを作成
	// *****************************************************************************************************************
	D3D11_BUFFER_DESC cbDesc = {};

	//2D頂点用
	::ZeroMemory(&cbDesc, sizeof(cbDesc));
	cbDesc.ByteWidth = sizeof(CBUFFER_2D);        // バッファサイズ
	cbDesc.Usage = D3D11_USAGE_DYNAMIC;       // リソース使用法を特定する
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;// バッファの種類
	cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;    // CPU アクセス
	cbDesc.MiscFlags = 0;                         // その他のフラグも設定しない
	hr = pD3DDevice.CreateBuffer(&cbDesc, NULL, &pBuffer);
	if (FAILED(hr)) return hr;

	mCBuffer.emplace_back(pBuffer);

	//3D頂点用
	::ZeroMemory(&cbDesc, sizeof(cbDesc));
	cbDesc.ByteWidth = sizeof(CBUFFER_3D);        // バッファサイズ
	cbDesc.Usage = D3D11_USAGE_DYNAMIC;       // リソース使用法を特定する
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;// バッファの種類
	cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;    // CPU アクセス
	cbDesc.MiscFlags = 0;                         // その他のフラグも設定しない
	hr = pD3DDevice.CreateBuffer(&cbDesc, NULL, &pBuffer);
	if (FAILED(hr)) return hr;
	mCBuffer.emplace_back(pBuffer);

	// *****************************************************************************************************************
	// インデックスバッファを作成
	// *****************************************************************************************************************
	D3D11_BUFFER_DESC ibDesc = {};
	D3D11_SUBRESOURCE_DATA InitData;
	::ZeroMemory(&ibDesc, sizeof(ibDesc));
	WORD indices[] =
	{
		  0,  1,  2,     3,  2,  1,
	 4,  5,  6,     7,  6,  5,
	 8,  9, 10,    11, 10,  9,
	12, 13, 14,    15, 14, 13,
	16, 17, 18,    19, 18, 17,
	20, 21, 22,    23, 22, 21,
	};
	ibDesc.Usage = D3D11_USAGE_DEFAULT;
	ibDesc.ByteWidth = sizeof(WORD) * 6 * 6;
	ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibDesc.CPUAccessFlags = 0;
	InitData.pSysMem = indices;
	hr = pD3DDevice.CreateBuffer(&ibDesc, &InitData, &pBuffer);
	if (FAILED(hr))
		return hr;

	mIBuffer.emplace_back(pBuffer);

	// インデックスバッファの設定
	pD3DDeviceContext.IASetIndexBuffer(mIBuffer[0], DXGI_FORMAT_R16_UINT, 0);

	g_World = XMMatrixIdentity();//ワールド座標系初期化

	// ビューマトリックスの初期化
	Eye = XMVectorSet(0.0f, 3.0f, -5.0f, 0.0f);
	At = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	g_View = XMMatrixLookAtLH(Eye, At, Up);

	// プロジェクションマトリックスの初期化
	g_Projection = XMMatrixPerspectiveFovLH(XM_PIDIV2, Define::WIN_W / (FLOAT)Define::WIN_H, 0.01f, 100.0f);

	//２D描画モード
	hr = ChangeMode_2D(pD3DDevice, pD3DDeviceContext);
	if (FAILED(hr))goto EXIT;

	Log("	シェーダー初期化完了\n");
	hr = S_OK;
EXIT:

	return hr;
}

//=====================================
// 描画モードを2Dにする
//=====================================
HRESULT BASE_SHADING::ChangeMode_2D(ID3D11Device& pD3DDevice, ID3D11DeviceContext& pD3DDeviceContext)
{
	HRESULT hr = E_FAIL;

	SetShader(pD3DDeviceContext, VERTEX_SHADER_Local, PIXEL_SHADER_2D_Raw);

	// プロミティブ・トポロジーをセット
	pD3DDeviceContext.IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// サンプラーステートを作成しセットする
	{
		// フィルタリングなし、Wrapモード
		D3D11_SAMPLER_DESC desc = {};
		desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;	// 何もフィルタリングしない
		desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;		// テクスチャアドレッシングモードをWrapに
		desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;		// テクスチャアドレッシングモードをWrapに
		desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;		// テクスチャアドレッシングモードをWrapに
		desc.MipLODBias = 0;
		desc.MaxAnisotropy = 0;
		desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		desc.BorderColor[0] = desc.BorderColor[1] = desc.BorderColor[2] = desc.BorderColor[3] = 0;
		desc.MinLOD = 0;
		desc.MaxLOD = D3D11_FLOAT32_MAX;

		// ステートオブジェクト作成
		ComPtr<ID3D11SamplerState> state;
		hr = pD3DDevice.CreateSamplerState(&desc, &state);
		if (FAILED(hr))return hr;
		// 各シェーダーの0番目にセット
		pD3DDeviceContext.VSSetSamplers(0, 1, state.GetAddressOf()); // 頂点シェーダーの0番目にセット
		pD3DDeviceContext.PSSetSamplers(0, 1, state.GetAddressOf()); // ピクセルシェーダーの0番目にセット
		pD3DDeviceContext.GSSetSamplers(0, 1, state.GetAddressOf()); // ジオメトリシェーダーの0番目にセット
		pD3DDeviceContext.CSSetSamplers(0, 1, state.GetAddressOf()); // コンピュートシェーダーの0番目にセット
	}

	hr = S_OK;
	return hr;
}

void BASE_SHADING::WriteVertexInfo2D(ID3D11DeviceContext& pD3DDeviceContext, Vertex2D& v, size_t arraySize)
{
	g_World = XMMatrixIdentity();//ワールド座標系初期化
	UINT stride = sizeof(Vertex2D);
	UINT offset = 0;
	pD3DDeviceContext.IASetVertexBuffers(0, 1, &mVBuffer[VERTEX_BUFFER_2D], &stride, &offset);
	// 頂点バッファにデータを書き込む
	D3D11_MAPPED_SUBRESOURCE pData;

	if (SUCCEEDED(pD3DDeviceContext.Map(mVBuffer[VERTEX_BUFFER_2D], 0, D3D11_MAP_WRITE_DISCARD, 0, &pData)))
	{
		// データコピー
		memcpy_s(pData.pData, arraySize, &v, arraySize);

		pD3DDeviceContext.Unmap(mVBuffer[VERTEX_BUFFER_2D], 0);
	}

	//定数バッファにデータを書き込む
	CBUFFER_2D cb;
	if (SUCCEEDED(pD3DDeviceContext.Map(mCBuffer[CONSTANT_BUFFER_2D], 0, D3D11_MAP_WRITE_DISCARD, 0, &pData)))
	{
		cb.matWVP = XMMatrixTranspose(g_World);
		cb.viewPortWidth = Define::WIN_W;
		cb.viewPortHeight = Define::WIN_H;
		cb.alpha = alpha;
		memcpy_s(pData.pData, pData.RowPitch, (void*)(&cb), sizeof(cb));
		pD3DDeviceContext.Unmap(mCBuffer[CONSTANT_BUFFER_2D], 0);
	}

	pD3DDeviceContext.VSSetConstantBuffers(0, 1, &mCBuffer[CONSTANT_BUFFER_2D]);
	pD3DDeviceContext.GSSetConstantBuffers(0, 1, &mCBuffer[CONSTANT_BUFFER_2D]);
	pD3DDeviceContext.PSSetConstantBuffers(0, 1, &mCBuffer[CONSTANT_BUFFER_2D]);
}

void BASE_SHADING::WriteVertexInfoInstancing2D(ID3D11DeviceContext& pD3DDeviceContext,
	InstanceData2D& v, size_t arraySize,const InstanceData2D& data, size_t dataSize)
{

	UINT strides[2] = { sizeof(InstanceData2D),sizeof(InstanceData2D) };
	UINT offsets[2] = { 0,0 };
	ID3D11Buffer* buffers[2] = { mVBuffer[VERTEX_BUFFER_Instancing2D_Original], mVBuffer[VERTEX_BUFFER_Instancing2D] };
	//頂点バッファとインスタンスバッファをバインド
	pD3DDeviceContext.IASetVertexBuffers(0, 2, buffers, strides, offsets);

	D3D11_MAPPED_SUBRESOURCE pData;

	// 頂点バッファにデータを書き込む(原本)
	if (SUCCEEDED(pD3DDeviceContext.Map(mVBuffer[VERTEX_BUFFER_Instancing2D_Original], 0, D3D11_MAP_WRITE_DISCARD, 0, &pData)))
	{
		memcpy_s(pData.pData, arraySize, &v, arraySize);//原本

		pD3DDeviceContext.Unmap(mVBuffer[VERTEX_BUFFER_Instancing2D_Original], 0);
	}
	// 頂点バッファにデータを書き込む(インスタンスバッファ)
	if (SUCCEEDED(pD3DDeviceContext.Map(mVBuffer[VERTEX_BUFFER_Instancing2D], 0, D3D11_MAP_WRITE_DISCARD, 0, &pData)))
	{
		memcpy_s(pData.pData, sizeof(InstanceData2D) * dataSize, &data, sizeof(InstanceData2D) * dataSize);//コピーするデータ

		pD3DDeviceContext.Unmap(mVBuffer[VERTEX_BUFFER_Instancing2D], 0);
	}

	g_World = XMMatrixIdentity();//ワールド座標系初期化

	//定数バッファにデータを書き込む
	CBUFFER_2D cb;
	if (SUCCEEDED(pD3DDeviceContext.Map(mCBuffer[CONSTANT_BUFFER_2D], 0, D3D11_MAP_WRITE_DISCARD, 0, &pData)))
	{
		cb.matWVP = XMMatrixTranspose(g_World);
		cb.viewPortWidth = Define::WIN_W;
		cb.viewPortHeight = Define::WIN_H;
		cb.alpha = alpha;
		memcpy_s(pData.pData, pData.RowPitch, (void*)(&cb), sizeof(cb));
		pD3DDeviceContext.Unmap(mCBuffer[CONSTANT_BUFFER_2D], 0);
	}
	pD3DDeviceContext.VSSetConstantBuffers(0, 1, &mCBuffer[CONSTANT_BUFFER_2D]);//通常の描画と同じ
	pD3DDeviceContext.GSSetConstantBuffers(0, 1, &mCBuffer[CONSTANT_BUFFER_2D]);
	pD3DDeviceContext.PSSetConstantBuffers(0, 1, &mCBuffer[CONSTANT_BUFFER_2D]);
}

void BASE_SHADING::WriteVertexInfo3D(ID3D11DeviceContext& pD3DDeviceContext, Vertex3D& v, size_t arraySize)
{
	g_World = XMMatrixIdentity();//ワールド座標系初期化
	UINT stride = sizeof(Vertex3D);
	UINT offset = 0;
	pD3DDeviceContext.IASetVertexBuffers(0, 1, &mVBuffer[VERTEX_BUFFER_3D], &stride, &offset);
	// 頂点バッファにデータを書き込む
	D3D11_MAPPED_SUBRESOURCE pData;
	if (SUCCEEDED(pD3DDeviceContext.Map(mVBuffer[VERTEX_BUFFER_3D], 0, D3D11_MAP_WRITE_DISCARD, 0, &pData)))
	{
		// データコピー
		memcpy_s(pData.pData, arraySize, &v, arraySize);

		pD3DDeviceContext.Unmap(mVBuffer[VERTEX_BUFFER_3D], 0);
	}

	g_World = XMMatrixTranslation(0, 0, 10);
	XMMATRIX rotate;
	rotate = XMMatrixRotationY(0);
	g_World = XMMatrixMultiply(rotate, g_World);
	rotate = XMMatrixRotationX(0);
	g_World = XMMatrixMultiply(rotate, g_World);
	//ライトの設定（一つだけ）
	XMVECTOR light = XMVectorSet(0.0f, 2.0f, -1.5f, 0.0f);
	XMVECTOR attenuation = XMVectorSet(0.0f, 0.3f, 0.2f, 0.0f);
	XMVECTOR lightColor = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);

	//定数バッファにデータを書き込む
	CBUFFER_3D cb;
	if (SUCCEEDED(pD3DDeviceContext.Map(mCBuffer[CONSTANT_BUFFER_3D], 0, D3D11_MAP_WRITE_DISCARD, 0, &pData)))
	{
		cb.mWorld = XMMatrixTranspose(g_World);
		cb.mView = XMMatrixTranspose(g_View);
		cb.mProjection = XMMatrixTranspose(g_Projection);
		XMStoreFloat4(&cb.light, light);
		XMStoreFloat4(&cb.lightColor, lightColor);
		XMStoreFloat4(&cb.attenuation, attenuation);
		XMStoreFloat4(&cb.eyePos, Eye);
		memcpy_s(pData.pData, pData.RowPitch, (void*)(&cb), sizeof(cb));
		pD3DDeviceContext.Unmap(mCBuffer[CONSTANT_BUFFER_3D], 0);
	}

	pD3DDeviceContext.VSSetConstantBuffers(0, 1, &mCBuffer[CONSTANT_BUFFER_3D]);
	pD3DDeviceContext.GSSetConstantBuffers(0, 1, &mCBuffer[CONSTANT_BUFFER_3D]);
	pD3DDeviceContext.PSSetConstantBuffers(0, 1, &mCBuffer[CONSTANT_BUFFER_3D]);
}


void BASE_SHADING::SetAlpha(int value)
{
	if (value < 0)value = 0; if (value > 255)value = 255;
	_value = value;
	float Alpha = (float)value / 255; alpha = Alpha;
}

int BASE_SHADING::GetAlpha()const
{
	return _value;
}

//シェーダーを設定する
void BASE_SHADING::SetShader
(
	ID3D11DeviceContext& pImmediateContext,
	BASE_VERTEXSHADER indexVS,
	BASE_PIXELSHADER indexPS,
	int indexGS,
	int indexHS,
	int indexDS,
	int indexCS
)
{
	assert(indexVS < (int)mVS.size());
	assert(indexPS < (int)mPS.size());
	assert(indexGS < (int)mGS.size());
	assert(indexHS < (int)mHS.size());
	assert(indexDS < (int)mDS.size());
	assert(indexCS < (int)mCS.size());

	ID3D11VertexShader* pVS = (indexVS >= 0) ? mVS[indexVS] : nullptr;
	ID3D11PixelShader* pPS = (indexPS >= 0) ? mPS[indexPS] : nullptr;
	ID3D11GeometryShader* pGS = (indexGS >= 0) ? mGS[indexGS] : nullptr;
	ID3D11HullShader* pHS = (indexHS >= 0) ? mHS[indexHS] : nullptr;
	ID3D11DomainShader* pDS = (indexDS >= 0) ? mDS[indexDS] : nullptr;
	ID3D11ComputeShader* pCS = (indexCS >= 0) ? mCS[indexCS] : nullptr;

	pImmediateContext.VSSetShader(pVS, NULL, 0);
	pImmediateContext.PSSetShader(pPS, NULL, 0);
	pImmediateContext.GSSetShader(pGS, NULL, 0);
	pImmediateContext.DSSetShader(pDS, NULL, 0);
	pImmediateContext.HSSetShader(pHS, NULL, 0);
	pImmediateContext.CSSetShader(pCS, NULL, 0);
}

void BASE_SHADING::SetInputLayout(ID3D11DeviceContext& pImmediateContext, BASE_LAYOUT index)
{
	assert(index < (int)mLayout.size());

	ID3D11InputLayout* pLayout = (index >= 0) ? mLayout[index] : nullptr;

	pImmediateContext.IASetInputLayout(pLayout);
}
