#include "Camera.h"
#include "DirectX.h"

HRESULT Camera::InitCamera()
{
	HRESULT hr = E_FAIL;
	D3D11_BUFFER_DESC BufferDesc;
	//カメラ用
	::ZeroMemory(&BufferDesc, sizeof(BufferDesc));
	BufferDesc.ByteWidth = sizeof(CBUFFER1);        // バッファサイズ
	BufferDesc.Usage = D3D11_USAGE_DYNAMIC;       // リソース使用法を特定する
	BufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;// バッファの種類
	BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;    // CPU アクセス
	BufferDesc.MiscFlags = 0;                         // その他のフラグも設定しない
	hr = D3D.GetDevice()->CreateBuffer(&BufferDesc, NULL, &m_pCameraConstantBuffer);
	if (FAILED(hr)) return hr;
	//初期設定
	SetCameraPos(0, 0); SetCameraRot(0); SetCameraScl(1,1);
	WriteCameraInfo2D();
	return S_OK;
}

void Camera::WriteCameraInfo2D()
{
	D3D11_MAPPED_SUBRESOURCE pData;
	//定数バッファにデータを書き込む
	if (SUCCEEDED(D3D.GetDeviceContext()->Map(m_pCameraConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &pData)))
	{
		memcpy_s(pData.pData, pData.RowPitch, (void*)(&cbuffer1), sizeof(cbuffer1));
		D3D.GetDeviceContext()->Unmap(m_pCameraConstantBuffer, 0);
	}

	D3D.GetDeviceContext()->VSSetConstantBuffers(1, 1, &m_pCameraConstantBuffer);
	
	D3D.GetDeviceContext()->PSSetConstantBuffers(1, 1, &m_pCameraConstantBuffer);
}

void Camera::SetCameraPos(float x, float y)
{
	cbuffer1.Pos.x = x;
	cbuffer1.Pos.y = y;
}

void Camera::SetCameraRot(float angle)
{
	cbuffer1.Rot = XMMatrixRotationZ(angle);
}

void Camera::SetCameraScl(float x, float y)
{
	if (x <= 0.01f)x = 0.01f;
	if (y <= 0.01f)y = 0.01f;
	cbuffer1.Scl = XMMatrixScaling(x, y, 1.0f);
}

void Camera::RemoveDevice()
{
	SAFE_RELEASE(m_pCameraConstantBuffer);
}