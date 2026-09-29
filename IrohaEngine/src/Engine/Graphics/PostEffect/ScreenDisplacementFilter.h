#pragma once
#include "PostEffect.h"
class PostEffect;

//スクリーンディスプレイスメントフィルタークラス：画面に対してディスプレイスメントテクスチャによる効果を適用するクラス。
class ScreenDisplacementFilter :public PostEffect
{
private:
	// ScreenDisplacementフィルターの定数バッファ定義
	typedef struct ALIGN16 _CBUFFER
	{
		float value;     //度合い

	}CBUFFER;

	float _value = 1;     

	//デバイス、デバイスコンテキスト
	ID3D11Device* m_pD3DDevice = nullptr;
	ID3D11DeviceContext* m_pD3DDeviceContext = nullptr;

	ID3D11VertexShader* m_pVertexShader = nullptr;// 頂点シェーダー
	ID3D11InputLayout* m_pLayout = nullptr;	// 入力レイアウト
	ID3D11Buffer* m_pVertexBuffer = nullptr;//頂点バッファ

	ID3D11PixelShader* m_pPixelShader = nullptr;// ピクセルシェーダー
	ID3D11Buffer* m_pConstantBuffer = nullptr;// 定数バッファ
	ID3D11RenderTargetView* m_pRTV = nullptr; // レンダー ターゲット ビュー



public:
	bool init(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int width, int height)override;//初期化
	void apply(IN ID3D11RenderTargetView* inputRTV, OUT ID3D11RenderTargetView* outputRTV)override;
	void setParameter(float hue, float saturation, float value);
	void cleanup()override;
	ScreenDisplacementFilter();
	virtual ~ScreenDisplacementFilter();
};
