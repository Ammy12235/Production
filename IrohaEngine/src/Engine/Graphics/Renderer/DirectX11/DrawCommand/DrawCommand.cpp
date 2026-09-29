#include "DrawCommand.h"
#include "Graphics/Texture/TextureFactory.h"
#include "Graphics/Renderer/DirectX11/DIRECT3D11.h"

shaderPipelineInfo shaderStruct[DrawComamnd_Max] =
{
	//なし
	{
		DrawComamnd_None,
		VERTEX_SHADER_None,
		PIXEL_SHADER_None,
		LAYOUT_None,
		D3D_PRIMITIVE_TOPOLOGY_UNDEFINED
	},
	//点
	{
		DrawComamnd_Point,
		VERTEX_SHADER_World,
		PIXEL_SHADER_2D_Raw,
		LAYOUT_2D,
		D3D11_PRIMITIVE_TOPOLOGY_POINTLIST
	},
	//線
	{
		DrawComamnd_Line,
		VERTEX_SHADER_World,
		PIXEL_SHADER_2D_Raw,
		LAYOUT_2D,
		D3D11_PRIMITIVE_TOPOLOGY_LINELIST
	},
	//矩形(色)
	{
		DrawComamnd_Rect_Color,
		VERTEX_SHADER_World,
		PIXEL_SHADER_2D_Raw,
		LAYOUT_2D,
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP
	},
	//矩形(テクスチャ)
	{
		DrawComamnd_Rect_Texture,
		VERTEX_SHADER_World,
		PIXEL_SHADER_2D_Texture,
		LAYOUT_2D,
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP
	},
	//矩形(SRV)
	{
		DrawComamnd_Rect_SRV,
		VERTEX_SHADER_World,
		PIXEL_SHADER_2D_Texture,
		LAYOUT_2D,
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP
	},
	//インスタンシング矩形(テクスチャ)
	{
		DrawComamnd_Rect_Texture_Instanced,
		VERTEX_SHADER_Instancing,
		PIXEL_SHADER_2D_Texture_UVOnly,
		LAYOUT_Instancing2D,
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP
	},
	//立方体(色)
	{
		DrawCommand_Cube,
		VERTEX_SHADER_3D,
		PIXEL_SHADER_3D_Color,
		LAYOUT_3D,
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST
	},

};
DrawCommand_2D::DrawCommand_2D(Command_2D* command_2D)
{
	command = *command_2D;
}

DrawCommand_2D_Instanced::DrawCommand_2D_Instanced(Command_2D_Instanced* command_2D_Instanced)
{
	command = *command_2D_Instanced;
}

DrawCommand_3D::DrawCommand_3D(Command_3D* command_3D)
{
	command = *command_3D;
}

void DrawCommand_2D::Execute(BASE_SHADING& pBaseShading, ID3D11DeviceContext& pDeviceContext)
{
	//ブレンドステートを設定
	D3D.SetBlendDesc(command.blendState, command.alpha);
	if (command.m_pDepthStencilState != nullptr)
	{
		D3D.IsSetDepthStencil(true);
		pDeviceContext.OMSetDepthStencilState(command.m_pDepthStencilState, 1);
	}
	//-----------------------------  
	// シェーダーをセット
	//----------------------------
	if (command.layer <= Layer_UI_2)//UI機能であればカメラの影響を受けないようにする
	{
		pBaseShading.SetShader(pDeviceContext,
			BASE_VERTEXSHADER::VERTEX_SHADER_Local,
			shaderStruct[command.drawCommand].pShader);
	}
	else
	{
		pBaseShading.SetShader(pDeviceContext,
			shaderStruct[command.drawCommand].vShader,
			shaderStruct[command.drawCommand].pShader);
	}
	pBaseShading.SetInputLayout(pDeviceContext, shaderStruct[command.drawCommand].layout);
	//頂点情報をバッファに書き込む
	pBaseShading.WriteVertexInfo2D(pDeviceContext, command.v[0], command.v.capacity() * sizeof(command.v[0]));

	if (command.drawCommand == DrawComamnd_Rect_Texture)//テクスチャならテクスチャを
	{
		Texture* tex = TEX_FAC.getTexturebyId(command.texId).get();
		// テクスチャを、スロット0にセット
		pDeviceContext.PSSetShaderResources(0, 1, tex->getTexResource());
	}
	else if (command.drawCommand == DrawComamnd_Rect_SRV)//SRVならSRVを設定
	{
		// テクスチャを、スロット0にセット
		pDeviceContext.PSSetShaderResources(0, 1, command.m_pSRV);
	}
	// プロミティブ・トポロジーをセット
	pDeviceContext.IASetPrimitiveTopology(shaderStruct[command.drawCommand].topology);

	pDeviceContext.Draw(command.vertexNum, 0);
	D3D.IsSetDepthStencil(false);
}

void DrawCommand_2D_Instanced::Execute(BASE_SHADING& pBaseShading, ID3D11DeviceContext& pDeviceContext)
{
	//ブレンドステートを設定
	D3D.SetBlendDesc(command.blendState, command.alpha);
	if (command.m_pDepthStencilState != nullptr)
	{
		D3D.IsSetDepthStencil(true);
		pDeviceContext.OMSetDepthStencilState(command.m_pDepthStencilState, 1);
	}
	//-----------------------------  
	// シェーダーをセット
	//----------------------------

	pBaseShading.SetShader(pDeviceContext,
		shaderStruct[command.drawCommand].vShader,
		shaderStruct[command.drawCommand].pShader);

	pBaseShading.SetInputLayout(pDeviceContext, shaderStruct[command.drawCommand].layout);
	//頂点情報をバッファに書き込む
	pBaseShading.WriteVertexInfoInstancing2D(pDeviceContext,
		command.v[0], command.v.capacity() * sizeof(command.v[0]),//基準となる形のデータ
		*command.instancedData, command.instanceNum);//インスタンスデータ

	//現状はテクスチャのみ
	Texture* tex = TEX_FAC.getTexturebyId(command.texId).get();
	// テクスチャを、スロット0にセット
	pDeviceContext.PSSetShaderResources(0, 1, tex->getTexResource());

	// プロミティブ・トポロジーをセット
	pDeviceContext.IASetPrimitiveTopology(shaderStruct[command.drawCommand].topology);

	pDeviceContext.DrawInstanced(command.v.size(), command.instanceNum ,0,0);
	D3D.IsSetDepthStencil(false);
}

void DrawCommand_3D::Execute(BASE_SHADING& pBaseShading, ID3D11DeviceContext& pDeviceContext)
{
	//ブレンドステートを設定
	D3D.SetBlendDesc(command.blendState, command.alpha);

	if (command.m_pDepthStencilState != nullptr)
	{
		D3D.IsSetDepthStencil(true);
		pDeviceContext.OMSetDepthStencilState(command.m_pDepthStencilState, 1);
	}
	pDeviceContext.OMSetDepthStencilState(command.m_pDepthStencilState, 1);
	//-----------------------------  
	// シェーダーをセット
	//----------------------------
	pBaseShading.SetShader(pDeviceContext,
		shaderStruct[command.drawCommand].vShader,
		shaderStruct[command.drawCommand].pShader);
	pBaseShading.SetInputLayout(pDeviceContext, shaderStruct[command.drawCommand].layout);
	//頂点情報をバッファに書き込む
	pBaseShading.WriteVertexInfo3D(pDeviceContext, command.v[0], command.v.capacity() * sizeof(command.v[0]));

	// プロミティブ・トポロジーをセット
	pDeviceContext.IASetPrimitiveTopology(shaderStruct[command.drawCommand].topology);

	pDeviceContext.DrawIndexed(36, 0, 0);
	D3D.IsSetDepthStencil(false);
}
