#pragma once
#include "Core/Singleton.h"
#include "Core/Window/WINDOW.h"

//Direct3D12クラス:各DirectX12描画用デバイスの初期化、管理、削除を行うクラス。
class DIRECT3D12 final :public Singleton<DIRECT3D12>
{
public:
	//===================================
	//初期化用関数
	//===================================
	HRESULT Init(D3D_INIT* pcd);
	HRESULT InitBackBuffer();
	//===================================
	//終了用関数
	//===================================
	void RemoveDevice();

	//===================================
	//設定用関数
	//===================================
	HRESULT SetGamma(float gamma);

	//===================================
	//スクリーンの更新と削除
	//===================================
	void Clear();
	HRESULT Present();

private:
	D3D12_VIEWPORT                      g_viewPort;
	D3D12_RECT                          g_rect;
	ComPtr<IDXGISwapChain3>               g_swapChain = nullptr;
	ComPtr<ID3D12Device>              g_device = nullptr;
	ComPtr<ID3D12Resource>                g_renderTargetArray[2];
	ComPtr<ID3D12CommandAllocator>        g_commandAllocator = nullptr;
	ComPtr<ID3D12CommandQueue>            g_commandQueue = nullptr;
	ComPtr<ID3D12RootSignature>           g_rootSignature = nullptr;
	ComPtr<ID3D12DescriptorHeap>      g_heap = nullptr;
	ComPtr<ID3D12DescriptorHeap>      g_constantHeap = nullptr;
	ComPtr<ID3D12PipelineState>           g_pipelineState = nullptr;
	ComPtr<ID3D12GraphicsCommandList> g_commandList = nullptr;
	UINT                                g_heapSize = 0;

	ComPtr<ID3D12Resource>                g_vertexBuffer = nullptr;
	D3D12_VERTEX_BUFFER_VIEW            g_vertexBufferView;

	ComPtr<ID3D12Resource>                g_constantBuffer = nullptr;
	UINT8* g_constantBufferData = nullptr;

	// 同期オブジェクト
	UINT                                g_frameIndex = 0;
	HANDLE                              g_fenceHandle;
	ComPtr<ID3D12Fence>                   g_fence = nullptr;
	UINT64                              g_fenceValue;

	float                               g_acpectRatio=0;

	bool                                g_useWarpDevice = false;

	DWORD                               g_startTime;

	//Method
	DIRECT3D12()
	{

	};
	virtual ~DIRECT3D12()
	{
		RemoveDevice();
	};

	void WaitForPreviousFrame();

public:

};

#define D3D12 DIRECT3D12::GetInstance()
