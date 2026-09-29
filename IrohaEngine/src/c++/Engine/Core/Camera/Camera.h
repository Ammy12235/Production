#pragma once
#include "Core/Singleton.h"
#include "Core/Entity.h"

#include "CameraShaker.h"

class Entity;

//カメラクラス：カメラの更新を行うクラス。
class Camera:public Singleton<Camera>
{
public:
	void SetCameraPos(XMFLOAT2 pos);
	void SetCameraRot(float angle);
	void SetCameraScl(float scale);
	void SetCameraTarget(Entity* target,float springConst);

	XMFLOAT2 GetCameraPos()const;
	float GetCameraRot()const;
	float GetCameraScl()const;
	bool update();
	void WriteCameraInfo2D();
	void RemoveDevice();

private:
		
	// カメラのスクリーン座標用の定数バッファ定義
	typedef struct ALIGN16 _CBUFFER1
	{
		XMMATRIX Rot;
		XMMATRIX Scl;
		XMFLOAT2 Pos;
	}CBUFFER1;

	//ターゲット
	Entity* _target;
	XMFLOAT2 _finalGround;
	bool isPreJump = false;

	float timerC = 0.0f;

	float _springConst = 0.0f;//ばね定数

	ResultShakeParameter resultShakeParameter;//カメラシェイクのパラメータを格納する構造体


	CBUFFER1 cbuffer1;
	//定数バッファ（カメラ用）
	ID3D11Buffer* m_pCameraConstantBuffer = nullptr;

	//カメラを揺らすために起点を作る
	XMFLOAT2 _pivotPos = {0,0};//基準の位置
	XMFLOAT2 _prePivotPos = {0,0};
	float _pivotRot = 0;//基準の回転
	float _pivotScale = 1.0f;//基準の拡大率

	HRESULT InitCamera();
public:
		//Method
	Camera()
	{
		InitCamera();
	};
	virtual ~Camera()
	{
		RemoveDevice();
	};
};

#define CAMERA Camera::GetInstance()
