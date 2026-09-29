#pragma once
#include "Base.h"

class ShaderStorage
{

private:
	//シェーダーコンテナ
	std::vector< ID3D11VertexShader* >		mVS;
	std::vector< ID3D11PixelShader* >		mPS;
	std::vector< ID3D11GeometryShader* >	mGS;
	std::vector< ID3D11HullShader* >		mHS;
	std::vector< ID3D11DomainShader* >		mDS;
	std::vector< ID3D11ComputeShader* >		mCS;
	std::vector< ID3D11InputLayout*>        mLayout;

public:
	void setVS(ID3D11VertexShader* vs);
	void setPS(ID3D11PixelShader* ps);
	void setGS(ID3D11GeometryShader* gs);
	void setHS(ID3D11HullShader* hs);
	void setDS(ID3D11DomainShader* ds);
	void setCS(ID3D11ComputeShader* cs);
	void setLayout(ID3D11InputLayout* layout);

	// 描画終了処理
	void CleanupDevice();
	void ReleseContainer();

	ShaderStorage();
	virtual ~ShaderStorage();
};