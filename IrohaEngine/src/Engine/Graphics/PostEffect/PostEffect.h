#pragma once
#include "Core/Base.h"

//ポストエフェクトクラス：ポストエフェクトの基底クラス。
class PostEffect
{
public:
	virtual bool init(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int width,int height)=0;//初期化
	virtual void apply(IN ID3D11RenderTargetView* inputRTV, OUT ID3D11RenderTargetView* outputRTV) = 0;//エフェクトを適用する
	virtual void setIsApply(bool isApply)//そのポストエフェクトを適用するか
	{
		_isApply = isApply;
	}
	virtual bool isApply()//ポストエフェクトが適用されているか
	{
		return _isApply == true ? true : false;
	}
	virtual void cleanup() = 0;//後処理
	virtual ~PostEffect()=default;
protected:
	bool _isApply = true;
};
