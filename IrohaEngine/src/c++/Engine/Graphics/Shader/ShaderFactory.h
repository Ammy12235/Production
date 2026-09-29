#pragma once
#include "Core/Singleton.h"

#include "ShaderUtility.h"
#include "ShaderBlob.h"

//==========================================
//シェーダー生成関連
//==========================================
//生成方法
enum eGenerateType
{
	FROM_NONE=-1,
	FROM_FILE,
	FROM_MEMORY,
	FROM_RESOURCE,
};

//シェーダ生成に関する情報
struct ShaderDesc
{
	std::string fileName;//ファイル名
	LPCSTR	entryPointName;//!< エントリーポイント名
	LPCSTR	shaderModel;//!< シェーダモデル名
};
//シェーダーファクトリークラス：シェーダを生成するクラス。
class ShaderFactory:public Singleton<ShaderFactory>
{
public:
	bool init(ID3D11Device* pD3DDevice, ID3D11DeviceContext* pD3DDeviceContext);
	
	// シェーダーファイルを読み込む
	HRESULT CreateVertexShader(
		eGenerateType generateType, 
		ShaderDesc shaderDesc, 
		OUT ID3D11VertexShader** ppShader,
		D3D11_INPUT_ELEMENT_DESC pLayoutDesc[],
		UINT NumElements,
		OUT ID3D11InputLayout** g_pLayout);
	HRESULT CreatePixelShader(
		eGenerateType generateType,
		ShaderDesc shaderDesc,
		OUT ID3D11PixelShader** ppShader);
	HRESULT CreateGeometryShader(
		eGenerateType generateType, 
		ShaderDesc shaderDesc,
		OUT ID3D11GeometryShader** ppShader);
	HRESULT CreateHullShader(
		eGenerateType generateType,
		ShaderDesc shaderDesc,
		OUT ID3D11HullShader** ppShader);
	HRESULT CreateDomainShader(
		eGenerateType generateType,
		ShaderDesc shaderDesc,
		OUT ID3D11DomainShader** ppShader);
	HRESULT CreateComputeShader(
		eGenerateType generateType,
		ShaderDesc shaderDesc,
		OUT ID3D11ComputeShader** ppShader);

private:

	ID3D11Device* _pD3DDevice = nullptr;
	ID3D11DeviceContext* _pD3DDeviceContext = nullptr;

	

};

#define SHADER_FAC ShaderFactory::GetInstance()
