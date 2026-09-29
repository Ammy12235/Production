#pragma once

#include <Iroha.h>

class WipeEffect
{
private:
	ID3D11DepthStencilState* m_pDepthStencilState = nullptr;
	ID3D11DepthStencilState* m_pDepthStencilState2 = nullptr;
	ID3D11Device* pd3dDevice;
	ID3D11DeviceContext* pd3dDeviceContext;

	XMFLOAT2 size;
	int texId = 0;
public:
	~WipeEffect() { RemoveDevice(); };
	WipeEffect() {};
	void RemoveDevice();
	void init(ID3D11Device* g_pd3dDevice, ID3D11DeviceContext* g_pd3dDeviceContext, UINT width, UINT height);
	bool update();
	bool drawWipeEffect(int x,int y,float value)const;
protected:

};
