#include "ShaderStorage.h"

ShaderStorage::ShaderStorage()
{
	mVS.clear();
	mPS.clear();
	mGS.clear();
	mHS.clear();
	mDS.clear();
	mCS.clear();
	mLayout.clear();

}

ShaderStorage::~ShaderStorage()
{
	CleanupDevice();
	ReleseContainer();
}

void ShaderStorage::CleanupDevice()
{

}

void ShaderStorage::ReleseContainer()
{

	mVS.clear();
	mLayout.clear();
	mPS.clear();
	mGS.clear();
	mHS.clear();
	mDS.clear();
	mCS.clear();
}

void ShaderStorage::setVS(ID3D11VertexShader* vs)
{
	mVS.push_back(vs);
}
void ShaderStorage::setPS(ID3D11PixelShader* ps)
{
	mPS.push_back(ps);
}
void ShaderStorage::setGS(ID3D11GeometryShader* gs)
{
	mGS.push_back(gs);
}
void ShaderStorage::setHS(ID3D11HullShader* hs)
{
	mHS.push_back(hs);
}
void ShaderStorage::setDS(ID3D11DomainShader* ds)
{
	mDS.push_back(ds);
}
void ShaderStorage::setCS(ID3D11ComputeShader* cs)
{
	mCS.push_back(cs);
}
void ShaderStorage::setLayout(ID3D11InputLayout* layout)
{
	mLayout.push_back(layout);
}