#pragma once
#include "Base.h"

#define ALIGN16 _declspec(align(16)) 

class Camera
{
public:
	HRESULT InitCamera();
	void SetCameraPos(float x, float y);
	void SetCameraRot(float angle);
	void SetCameraScl(float x,float y);
	void WriteCameraInfo2D();
	void RemoveDevice();

private:
	// カメラのスクリーン座標用の定数バッファ定義
	typedef struct _CBUFFER1
	{
		ALIGN16 XMMATRIX Rot;
		ALIGN16 XMMATRIX Scl;
		ALIGN16 XMFLOAT2 Pos;
	}CBUFFER1;

	CBUFFER1 cbuffer1;
	//定数バッファ（カメラ用）
	ID3D11Buffer* m_pCameraConstantBuffer = nullptr;


	//Method
	Camera()
	{

	};
	virtual ~Camera()
	{
		RemoveDevice();
	};

	Camera(const  Camera& r) = default;
	Camera& operator=(const  Camera & r) = default;

	static inline Camera* s_instance;


public:

	static void CreateInstance() {
		if (!s_instance) {
			s_instance = new Camera;
		}
	}

	static void DeleteInstance() {
		delete s_instance;
		s_instance = nullptr;
	}
	static Camera& GetInstance() {
		return *s_instance;
	}
protected:
};

#define CAMERA Camera::GetInstance()