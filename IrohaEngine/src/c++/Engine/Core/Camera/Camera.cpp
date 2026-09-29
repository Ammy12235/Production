#include "Camera.h"
#include "Core/DEFINE.h"
#include "Graphics/Renderer/DirectX11/DIRECT3D11.h"

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
	SetCameraPos(XMFLOAT2(0,0)); SetCameraRot(0); SetCameraScl(1);
	
	WriteCameraInfo2D();
	return S_OK;
}

bool Camera::update()
{

	//カメラシェイクの更新
	CameraShaker::GetInstance().Update();
	resultShakeParameter = CameraShaker::GetInstance().GetShakeOffset();//結果を取得

	//位置回転スケールを三点まとめてカメラシェイクのオフセットも加算して更新
	cbuffer1.Pos = _pivotPos+ resultShakeParameter.resultPositionOffset;
	cbuffer1.Rot = XMMatrixRotationZ(_pivotRot+ resultShakeParameter.resultRotationOffset);
	float scale = _pivotScale + resultShakeParameter.resultScaleOffset;
	if (scale < 0.01f)scale = 0.01f;
	cbuffer1.Scl = XMMatrixScaling(scale, scale, 1.0f);

	WriteCameraInfo2D();//カメラの情報を定数バッファに書き込む
	return true;
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
	D3D.GetDeviceContext()->GSSetConstantBuffers(1, 1, &m_pCameraConstantBuffer);
	D3D.GetDeviceContext()->PSSetConstantBuffers(1, 1, &m_pCameraConstantBuffer);
}


void Camera::SetCameraPos(XMFLOAT2 pos)
{
	_pivotPos.x = floor(pos.x);
	_pivotPos.y = floor(pos.y);
	
}

void Camera::SetCameraRot(float angle)
{
	
	_pivotRot = angle;
	
}

void Camera::SetCameraScl(float scale)
{
	if (scale <= 0.01f)scale = 0.01f;
	_pivotScale = scale;
	
}

void Camera::SetCameraTarget(Entity* target, float springConst)
{
	_target = target;
	_springConst = springConst;

	_pivotPos = _target->GetPosition();
}


XMFLOAT2 Camera::GetCameraPos()const
{
	return _pivotPos;

}

float Camera::GetCameraRot()const
{
	return _pivotRot;
}

float Camera::GetCameraScl()const
{
	return _pivotScale;
}

void Camera::RemoveDevice()
{
	SAFE_RELEASE(m_pCameraConstantBuffer);
}
