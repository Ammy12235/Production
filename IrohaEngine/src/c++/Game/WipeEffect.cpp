#include "WipeEffect.h"

void WipeEffect::init(ID3D11Device* g_pd3dDevice, ID3D11DeviceContext* g_pd3dDeviceContext, UINT width, UINT height)
{
	HRESULT hr = E_FAIL;

	pd3dDevice = g_pd3dDevice;
	pd3dDeviceContext = g_pd3dDeviceContext;

	D3D11_DEPTH_STENCIL_DESC desc = {};
	//ステンシルテストを行う深度ステンシルステートの作成
	desc.DepthEnable = false;//既存ポリゴンをはさんでいる判断に必要
	//desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;//ただし自分自身でZテストしないために書き込みはゼロ
	//desc.DepthFunc = D3D11_COMPARISON_LESS;
	//ステンシルテスト
	desc.StencilEnable = true;
	desc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	desc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
	//表面をどうするか
	desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_REPLACE;
	desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
	desc.FrontFace.StencilFunc = D3D11_COMPARISON_NOT_EQUAL;
	//裏面をどうするか
	desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_REPLACE;
	desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
	desc.BackFace.StencilFunc = D3D11_COMPARISON_NOT_EQUAL;


	hr = pd3dDevice->CreateDepthStencilState(&desc, &m_pDepthStencilState);
	if (FAILED(hr)) {
		MSG(L"ステンシルテスト用のステート作成に失敗");
	}
	//ステンシルテストを行う深度ステンシルステートの作成
	desc.DepthEnable = false;
	desc.StencilEnable = true;
	desc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	desc.StencilWriteMask = 0x00;
	//表面をどうするか
	desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_ZERO;
	desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	desc.FrontFace.StencilFunc = D3D11_COMPARISON_NOT_EQUAL;
	//裏面をどうするか
	desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_ZERO;
	desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	desc.BackFace.StencilFunc = D3D11_COMPARISON_NOT_EQUAL;

	hr = pd3dDevice->CreateDepthStencilState(&desc, &m_pDepthStencilState2);
	if (FAILED(hr)) {
		MSG(L"ステンシルテスト用のステート作成に失敗");
	}
	texId=TEX_FAC.CreateTexture("mask.png");
	size.x = 0.0f;
	size.y = 0.0f;
}
void WipeEffect::RemoveDevice()
{
	SAFE_RELEASE(m_pDepthStencilState);
	SAFE_RELEASE(m_pDepthStencilState2);
}

bool WipeEffect::update()
{

	return true;
}

bool WipeEffect::drawWipeEffect(int x, int y, float value)const
{
	POINT p;
	//ScreenToClient(, &p);
	GetCursorPos(&p);
	ID3D11DepthStencilState* oldDSS = NULL;
	UINT Num = 0;
	D3D.IsSetDepthStencil(true);
	//現在のレンダリングステートを退避
	pd3dDeviceContext->OMGetDepthStencilState(&oldDSS, &Num);

	//レンダリングステートを設定
	pd3dDeviceContext->OMSetDepthStencilState(m_pDepthStencilState, 1);
	//ステンシルを作成（型を作る）
	D3D.SetAlignmentBlendDesc(255);
	D3D.DrawRotImage(texId, 640, 450, p.x, p.x, 0, 1);

	//レンダリングステートを設定
	pd3dDeviceContext->OMSetDepthStencilState(m_pDepthStencilState2, 1);
	D3D.DrawBox(0, 0, Define::WIN_W, Define::WIN_H, D3D.GetColor(0, 0, 0));
	D3D.SetDefaultBlendDesc(255);
	// レンダリングステートを元の状態に戻す
	pd3dDeviceContext->OMSetDepthStencilState(oldDSS, Num);
	D3D.IsSetDepthStencil(false);
	SAFE_RELEASE(oldDSS);

	return true;
}
