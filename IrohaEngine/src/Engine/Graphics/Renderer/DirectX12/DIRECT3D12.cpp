#include "DIRECT3D12.h"
#include "d3dx12.h"

HRESULT DIRECT3D12::Init(D3D_INIT* pcd)
{

	HRESULT hr = E_FAIL;

	RECT rc;
	GetClientRect(WINDOW::m_hWnd, &rc);//画面の幅、高さを取得、格納する
	UINT width = rc.right - rc.left;
	UINT height = rc.bottom - rc.top;
#ifdef _DEBUG

	// DirectX12のデバッグレイヤー有効化
	{
		ComPtr<ID3D12Debug> debugController = nullptr;
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
		{
			debugController->EnableDebugLayer();
		}
	}

#endif

	D3D_FEATURE_LEVEL featureLevels[] =//機能レベル
	{
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,	// Direct3D 11.1  ShaderModel 5
		D3D_FEATURE_LEVEL_11_0,	// Direct3D 11    ShaderModel 5
	};

	{
		// ハードウェアアダプタ取得
		ComPtr<IDXGIFactory4> factory = nullptr;
		hr = CreateDXGIFactory1(IID_PPV_ARGS(&factory));
		if (FAILED(hr))return hr;


		if (g_useWarpDevice)
		{
			ComPtr<IDXGIAdapter> warpAdapter = nullptr;
			hr = factory->EnumWarpAdapter(IID_PPV_ARGS(&warpAdapter));
			if (FAILED(hr))return hr;
			hr = D3D12CreateDevice(warpAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&g_device));
			if (FAILED(hr))return hr;
		}
		else
		{
			ComPtr<IDXGIAdapter1> hardwareAdapter = nullptr;
			ComPtr<IDXGIAdapter1> adapter = nullptr;

			for (UINT i = 0; DXGI_ERROR_NOT_FOUND != factory->EnumAdapters1(i, &adapter); i++)
			{
				DXGI_ADAPTER_DESC1 desc;
				adapter->GetDesc1(&desc);
				if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
				{
					continue;
				}
				if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), featureLevels[i], __uuidof(ID3D12Device), nullptr)))
				{
					break;
				}
			}

			hardwareAdapter = adapter.Detach();

			hr = D3D12CreateDevice(hardwareAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&g_device));
			if (FAILED(hr))return hr;
		}
		// コマンドキュー生成
		D3D12_COMMAND_QUEUE_DESC queueDesc = {};
		queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

		hr = g_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&g_commandQueue));
		if (FAILED(hr))return hr;

		// スワップチェイン生成
		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
		swapChainDesc.BufferCount = 2;
		swapChainDesc.Width = width;
		swapChainDesc.Height = height;
		swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapChainDesc.SampleDesc.Count = 1;

		ComPtr<IDXGISwapChain1> swapChain = nullptr;
		hr = factory->CreateSwapChainForHwnd(g_commandQueue.Get(), pcd->hWnd, &swapChainDesc, nullptr, nullptr, &swapChain);
		if (FAILED(hr))return hr;
		hr = factory->MakeWindowAssociation(pcd->hWnd, DXGI_MWA_NO_ALT_ENTER);
		if (FAILED(hr))return hr;
		hr = swapChain.As(&g_swapChain);
		if (FAILED(hr))return hr;

		g_frameIndex = g_swapChain->GetCurrentBackBufferIndex();
	}

	// ヒープ生成
	{
		D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
		heapDesc.NumDescriptors = 2;
		heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

		hr = g_device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&g_heap));
		if (FAILED(hr))return hr;

		g_heapSize = g_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	}

	// レンダーターゲットビュー生成
	{
		D3D12_CPU_DESCRIPTOR_HANDLE handle = {};
		handle.ptr = g_heap->GetCPUDescriptorHandleForHeapStart().ptr;

		for (UINT i = 0; i < 2; i++)
		{
			g_swapChain->GetBuffer(i, IID_PPV_ARGS(&g_renderTargetArray[i]));
			if (FAILED(hr))return hr;
			g_device->CreateRenderTargetView(g_renderTargetArray[i].Get(), nullptr, handle);
			handle.ptr += g_heapSize;
		}
	}

	// コマンドアロケータ生成
	if (hr = g_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&g_commandAllocator)))
	{
		return hr;
	}

	{
		// 定数バッファ生成
		D3D12_DESCRIPTOR_HEAP_DESC constantDesc = {};
		constantDesc.NumDescriptors = 1;
		constantDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		constantDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

		hr = g_device->CreateDescriptorHeap(&constantDesc, IID_PPV_ARGS(&g_constantHeap));
		if (FAILED(hr))return hr;

		// 定数バッファリソース生成
		D3D12_HEAP_PROPERTIES constantProperties = {};
		constantProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
		constantProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		constantProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		constantProperties.CreationNodeMask = 1;
		constantProperties.VisibleNodeMask = 1;

		D3D12_RESOURCE_DESC constantResource = {};
		constantResource.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		constantResource.Alignment = 0;
		constantResource.Width = 256;
		constantResource.Height = 1;
		constantResource.DepthOrArraySize = 1;
		constantResource.MipLevels = 1;
		constantResource.Format = DXGI_FORMAT_UNKNOWN;
		constantResource.SampleDesc = { 1, 0 };
		constantResource.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		constantResource.Flags = D3D12_RESOURCE_FLAG_NONE;

		hr = g_device->CreateCommittedResource(&constantProperties, D3D12_HEAP_FLAG_NONE, &constantResource, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&g_constantBuffer));
		if (FAILED(hr))return hr;

		// 定数バッファビュー生成
		D3D12_CONSTANT_BUFFER_VIEW_DESC constantView = {};
		constantView.BufferLocation = g_constantBuffer->GetGPUVirtualAddress();
		constantView.SizeInBytes = 256;
		g_device->CreateConstantBufferView(&constantView, g_constantHeap->GetCPUDescriptorHandleForHeapStart());

		// マップ
		g_constantBuffer->Map(0, nullptr, reinterpret_cast<void**>(&g_constantBufferData));
	}

	{
		// 定数バッファをルートシグネチャパラメータへ設定
		D3D12_DESCRIPTOR_RANGE  rangeArray[1];
		D3D12_ROOT_PARAMETER    rootParameterArray[1];
		rangeArray[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
		rangeArray[0].NumDescriptors = 1;
		rangeArray[0].BaseShaderRegister = 0;
		rangeArray[0].RegisterSpace = 0;
		rangeArray[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
		rootParameterArray[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		rootParameterArray[0].DescriptorTable.NumDescriptorRanges = 1;
		rootParameterArray[0].DescriptorTable.pDescriptorRanges = rangeArray;
		rootParameterArray[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

		// ルートシグネチャ生成
		D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
		rootSignatureDesc.NumParameters = _countof(rootParameterArray);
		rootSignatureDesc.pParameters = rootParameterArray;
		rootSignatureDesc.NumStaticSamplers = 0;
		rootSignatureDesc.pStaticSamplers = nullptr;
		rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

		ComPtr<ID3DBlob> signature = nullptr;
		ComPtr<ID3DBlob> error = nullptr;

		hr = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error);
		if (FAILED(hr))return hr;
		hr = g_device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&g_rootSignature));
		if (FAILED(hr))return hr;
	}
	/*
	{
		// シェーダーコンパイル
		ComPtr<ID3DBlob>  vertexShader = nullptr;
		ComPtr<ID3DBlob>  pixelShader = nullptr;
		UINT                compileFlags = 0;

		if (FAILED(D3DCompileFromFile(L"resource/sample.hlsl", nullptr, nullptr, "VSMain", "vs_5_0", compileFlags, 0, &vertexShader, nullptr)))
		{
			return false;
		}
		if (FAILED(D3DCompileFromFile(L"resource/sample.hlsl", nullptr, nullptr, "PSMain", "ps_5_0", compileFlags, 0, &pixelShader, nullptr)))
		{
			return false;
		}

		// 頂点入力レイアウト定義
		D3D12_INPUT_ELEMENT_DESC inputElementDescs[] = {
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT   , 0,  0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
			{"COLOR"   , 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
		};

		// グラフィックスパイプラインステート生成
		D3D12_GRAPHICS_PIPELINE_STATE_DESC gpsDesc = {};
		gpsDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
		gpsDesc.pRootSignature = g_rootSignature.Get();
		{
			// 頂点シェーダー
			D3D12_SHADER_BYTECODE shaderBytecode;
			shaderBytecode.pShaderBytecode = vertexShader->GetBufferPointer();
			shaderBytecode.BytecodeLength = vertexShader->GetBufferSize();
			gpsDesc.VS = shaderBytecode;
		}
		{
			// ピクセルシェーダー
			D3D12_SHADER_BYTECODE shaderBytecode;
			shaderBytecode.pShaderBytecode = pixelShader->GetBufferPointer();
			shaderBytecode.BytecodeLength = pixelShader->GetBufferSize();
			gpsDesc.PS = shaderBytecode;
		}
		{
			// ラスタライザ
			D3D12_RASTERIZER_DESC rasterizerDesc = {};
			rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
			rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
			rasterizerDesc.FrontCounterClockwise = false;
			rasterizerDesc.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
			rasterizerDesc.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
			rasterizerDesc.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
			rasterizerDesc.DepthClipEnable = true;
			rasterizerDesc.MultisampleEnable = false;
			rasterizerDesc.AntialiasedLineEnable = false;
			rasterizerDesc.ForcedSampleCount = 0;
			rasterizerDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
			gpsDesc.RasterizerState = rasterizerDesc;
		}
		{
			// ブレンド
			D3D12_BLEND_DESC blendDesc = {};
			blendDesc.AlphaToCoverageEnable = false;
			blendDesc.IndependentBlendEnable = false;
			for (UINT i = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; i++)
			{
				blendDesc.RenderTarget[i].BlendEnable = false;
				blendDesc.RenderTarget[i].LogicOpEnable = false;
				blendDesc.RenderTarget[i].SrcBlend = D3D12_BLEND_ONE;
				blendDesc.RenderTarget[i].DestBlend = D3D12_BLEND_ZERO;
				blendDesc.RenderTarget[i].BlendOp = D3D12_BLEND_OP_ADD;
				blendDesc.RenderTarget[i].SrcBlendAlpha = D3D12_BLEND_ONE;
				blendDesc.RenderTarget[i].DestBlendAlpha = D3D12_BLEND_ZERO;
				blendDesc.RenderTarget[i].BlendOpAlpha = D3D12_BLEND_OP_ADD;
				blendDesc.RenderTarget[i].LogicOp = D3D12_LOGIC_OP_NOOP;
				blendDesc.RenderTarget[i].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
			}
			gpsDesc.BlendState = blendDesc;
		}
		gpsDesc.DepthStencilState.DepthEnable = false;
		gpsDesc.DepthStencilState.StencilEnable = false;
		gpsDesc.SampleMask = UINT_MAX;
		gpsDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		gpsDesc.NumRenderTargets = 1;
		gpsDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		gpsDesc.SampleDesc.Count = 1;

		if (FAILED(g_device->CreateGraphicsPipelineState(&gpsDesc, IID_PPV_ARGS(&g_pipelineState))))
		{
			return false;
		}
	}
	*/

	// コマンドリスト生成
	{
		hr = g_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, g_commandAllocator.Get(), g_pipelineState.Get(), IID_PPV_ARGS(&g_commandList));
		if (FAILED(hr))return hr;
		hr = g_commandList->Close();
		if (FAILED(hr))return hr;
	}

	/*
	// 頂点バッファ生成
	{
		// ジオメトリ定義
		Vertex vertexArray[] = {
			{{ 0.f  ,  0.25f * g_acpectRatio, 0.f}, {1.f, 0.f, 0.f, 1.f}},
			{{ 0.25f, -0.25f * g_acpectRatio, 0.f}, {0.f, 1.f, 0.f, 1.f}},
			{{-0.25f, -0.25f * g_acpectRatio, 0.f}, {0.f, 0.f, 1.f, 1.f}}
		};

		UINT vertexBufferSize = sizeof(vertexArray);

		D3D12_HEAP_PROPERTIES heapProperties = {};
		heapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
		heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		heapProperties.CreationNodeMask = 1;
		heapProperties.VisibleNodeMask = 1;

		D3D12_RESOURCE_DESC resourceDesc = {};
		resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		resourceDesc.Alignment = 0;
		resourceDesc.Width = vertexBufferSize;
		resourceDesc.Height = 1;
		resourceDesc.DepthOrArraySize = 1;
		resourceDesc.MipLevels = 1;
		resourceDesc.Format = DXGI_FORMAT_UNKNOWN;
		resourceDesc.SampleDesc.Count = 1;
		resourceDesc.SampleDesc.Quality = 0;
		resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		resourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

		if (FAILED(g_device->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&g_vertexBuffer))))
		{
			return false;
		}

		// データ設定
		UINT8* pVertexDataBegin;
		D3D12_RANGE readRange = { 0, 0 };
		if (FAILED(g_vertexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pVertexDataBegin))))
		{
			return false;
		}
		memcpy(pVertexDataBegin, vertexArray, vertexBufferSize);
		g_vertexBuffer->Unmap(0, nullptr);

		// ビュー初期化
		g_vertexBufferView.BufferLocation = g_vertexBuffer->GetGPUVirtualAddress();
		g_vertexBufferView.StrideInBytes = sizeof(Vertex);
		g_vertexBufferView.SizeInBytes = vertexBufferSize;
	}
	*/

	// 同期オブジェクトを生成してリソースがGPUに転送されるまで待機する
	{
		hr = g_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&g_fence));
		if (FAILED(hr))return hr;

		g_fenceValue = 1;

		g_fenceHandle = CreateEvent(nullptr, false, false, nullptr);
		if (!g_fenceHandle)
		{
			hr = HRESULT_FROM_WIN32(GetLastError());
			if (FAILED(hr))return hr;
		}
	}

	g_startTime = timeGetTime();

	return S_OK;
}

HRESULT DIRECT3D12::Present()
{

	HRESULT hr=E_FAIL;
	// コマンドアロケータをリセット
	hr=g_commandAllocator->Reset();
	if (FAILED(hr))return hr;
	// コマンドリストをリセット
	hr=g_commandList->Reset(g_commandAllocator.Get(), g_pipelineState.Get());
	if (FAILED(hr))return hr;
	// バックバッファをレンダーターゲットとして使用
	auto startResourceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(g_renderTargetArray[g_frameIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	g_commandList->ResourceBarrier(1, &startResourceBarrier);
	// リソースバリアとは、GPU側で扱うリソースの状況を同期させる機能。
	// マルチスレッドを前提とした動きなので、GPU側の動作も複数のアクセスが同時に行われることを想定した機能だということ。

	// レンダーターゲットビューのハンドルを作成
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(g_heap->GetCPUDescriptorHandleForHeapStart(), g_frameIndex, g_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV));

	// バックバッファに描画(コマンドを記録)
	const FLOAT	clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };		// 青っぽい色
	g_commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);

	// バックバッファを表示
	auto endResourceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(g_renderTargetArray[g_frameIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	g_commandList->ResourceBarrier(1, &endResourceBarrier);

	// コマンドリストをクローズ
	 hr=g_commandList->Close();
	 if (FAILED(hr))return hr;

	// コマンドリストを実行
	ID3D12CommandList* ppCommandLists[] = { g_commandList.Get() };
	g_commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

	// フレームを最終出力
	hr=g_swapChain->Present(1, 0);
	if (FAILED(hr))return hr;

	// GPU処理の終了を待機
	{
		WaitForPreviousFrame();
	}
	return S_OK;

}
void DIRECT3D12::WaitForPreviousFrame()
{
	const UINT64 fence = g_fenceValue;
	g_commandQueue->Signal(g_fence.Get(), fence);
	g_fenceValue++;

	// 前のフレームが終了するまで待機
	if (g_fence->GetCompletedValue() < fence) {
		g_fence->SetEventOnCompletion(fence, g_fenceHandle);
		WaitForSingleObject(g_fenceHandle, INFINITE);
	}

	// バックバッファのインデックスを格納
	g_frameIndex = g_swapChain->GetCurrentBackBufferIndex();

}
void DIRECT3D12::RemoveDevice()
{
	WaitForPreviousFrame();
	CloseHandle(g_fenceHandle);
}
