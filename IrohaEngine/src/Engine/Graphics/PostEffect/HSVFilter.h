#pragma once
#include "PostEffect.h"
class PostEffect;

//HSVフィルタークラス：色彩、彩度、明度によって画面色を補正するクラス。
class HSVFilter :public PostEffect
{
private:
	// HSVフィルターの定数バッファ定義
	typedef struct ALIGN16 _CBUFFER
	{
		float hue;       //色相
		float saturation;//彩度
		float value;     //明度

	}CBUFFER;


	//色空間パラメータ
	float _hue = 1;       //色相
	float _saturation = 1;//彩度
	float _value = 1;     //明度

	
	ID3D11VertexShader* m_pVertexShader = nullptr;// 頂点シェーダー
	ID3D11PixelShader* m_pPixelShader = nullptr;// ピクセルシェーダー
	ID3D11InputLayout* m_pLayout = nullptr;	// 入力レイアウト
	ID3D11Buffer* m_pVertexBuffer = nullptr;//頂点バッファ
	ID3D11Buffer* m_pConstantBuffer = nullptr;// 定数バッファ
	ID3D11RenderTargetView* m_pRTV = nullptr; // レンダー ターゲット ビュー

	//デバイス、デバイスコンテキスト
	ID3D11Device* m_pD3DDevice = nullptr;
	ID3D11DeviceContext* m_pD3DDeviceContext = nullptr;

	
public:
	bool init(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int width, int height)override;//初期化
	void apply(IN ID3D11RenderTargetView* inputRTV, OUT ID3D11RenderTargetView* outputRTV)override;
	void setParameter(float hue, float saturation, float value);
	XMFLOAT3 getParameter()const
	{
		return XMFLOAT3(_hue, _saturation, _value);
	}
	void cleanup()override;
	HSVFilter();
	virtual ~HSVFilter();
};
