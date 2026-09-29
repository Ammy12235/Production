#pragma once
#include "PostEffect.h"

class PostEffect;

//ポストエフェクトチェインクラス：ポストエフェクトを連続的に適用するクラス。
class PostEffectChain
{
private:
	std::vector<PostEffect*> _effects;//ポストエフェクト群
	ID3D11RenderTargetView* _intermediateRTV1=nullptr;//チェインする中間RTV
	ID3D11RenderTargetView* _intermediateRTV2=nullptr;

	ID3D11Device* _device=nullptr;
	ID3D11DeviceContext* _deviceContext=nullptr;

	ID3D11VertexShader* _pVertexShader = nullptr;// 頂点シェーダー
	ID3D11PixelShader* _pPixelShader = nullptr;// ピクセルシェーダー
	ID3D11InputLayout* _pLayout = nullptr;	// 入力レイアウト
	ID3D11Buffer* _pVertexBuffer = nullptr;//頂点バッファ
	ID3D11Buffer* _pConstantBuffer = nullptr;// 定数バッファ
	ID3D11RenderTargetView* _pRTV = nullptr; // レンダー ターゲット ビュー

	int applyCount = 0;
public:
	bool init(ID3D11Device* device, ID3D11DeviceContext* deviceContext,
		int width, int height);
	void addEffect(PostEffect* postEffect);
	void apply(IN ID3D11RenderTargetView* inputRTV, OUT ID3D11RenderTargetView* outputRTV);
	int getApplyCount()const
	{
		return applyCount;
	};
	void cleanup();
	PostEffectChain()=default;
	virtual ~PostEffectChain();
};