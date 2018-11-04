#include "Render.h"
#include "Framework.h"

using namespace DirectX;

COMPONENT_DEFINE(Render, Render)

Render::Render(std::wstring name)
:	Component(name),
	m_TechName(""),
	m_DiffuseMapSRV(NULL),
	m_CubeMapSRV(NULL),
	m_RenderState(NULL),
	m_BlendState(NULL),
	m_BlendFactor(nullptr),
	m_SampleMask(0xFFFFFFFF),
	m_DepthStencilState(NULL),
	m_StencilRef(0),
	m_RenderStateEnable(true),
	m_BlendStateEnable(true),
	m_DepthStencilStateEnable(true)
{
	XMMATRIX I = XMMatrixIdentity();
	XMStoreFloat4x4(&m_World, I);
	XMStoreFloat4x4(&m_WorldViewProj, I);
	XMStoreFloat4x4(&m_TexTransform, I);
}

Render::~Render()
{
}

void Render::RenderStateEnable(bool enable)
{
	m_RenderStateEnable = enable;
}

void Render::BlendStateEnable(bool enable)
{
	m_BlendStateEnable = enable;
}

void Render::DepthStencilStateEnable(bool enable)
{
	m_DepthStencilStateEnable = enable;
}

void Render::SetEffectTechnique(const char *tech)
{
	m_TechName = tech;
}

void Render::Apply(UINT indexCount)
{
	Framework &fw = Framework::Instance();
	ID3DX11EffectTechnique *tech = Framework::BasicFX->GetTechniqueByName(m_TechName.c_str());

	if (!tech)
	{
		tech = Framework::BasicFX->Light0Tech;
	}

	// FX变量配置
	Framework::BasicFX->SetWorld(XMLoadFloat4x4(&m_World));
	Framework::BasicFX->SetWorldInvTranspose(MathHelper::InverseTranspose(XMLoadFloat4x4(&m_World)));
	Framework::BasicFX->SetWorldViewProj(XMLoadFloat4x4(&m_WorldViewProj));
	Framework::BasicFX->SetTexTransform(XMLoadFloat4x4(&m_TexTransform));
	Framework::BasicFX->SetMaterial(m_Material);
	if (m_DiffuseMapSRV)	Framework::BasicFX->SetDiffuseMap(m_DiffuseMapSRV);
	if (m_CubeMapSRV)		Framework::BasicFX->SetCubeMap(m_CubeMapSRV);
	// 设置特殊状态
	if (m_RenderStateEnable && m_RenderState) fw.DeviceContext()->RSSetState(m_RenderState);
	if (m_BlendStateEnable && m_BlendState) fw.DeviceContext()->OMSetBlendState(m_BlendState, m_BlendFactor, m_SampleMask);
	if (m_DepthStencilStateEnable && m_DepthStencilState) fw.DeviceContext()->OMSetDepthStencilState(m_DepthStencilState, m_StencilRef);
	
	D3DX11_TECHNIQUE_DESC techDesc;
	tech->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		tech->GetPassByIndex(p)->Apply(0, fw.DeviceContext());
		fw.DeviceContext()->DrawIndexed(indexCount, 0, 0);
	}

	// 恢复默认状态
	if (m_RenderStateEnable && m_RenderState)		fw.DeviceContext()->RSSetState(NULL);
	if (m_BlendStateEnable && m_BlendState)			fw.DeviceContext()->OMSetBlendState(NULL, m_BlendFactor, 0xFFFFFFFF);
	if (m_DepthStencilStateEnable && m_DepthStencilState)	fw.DeviceContext()->OMSetDepthStencilState(NULL, m_StencilRef);
}

void Render::SetWorld(CXMMATRIX &M)
{
	XMStoreFloat4x4(&m_World, M);
}

void Render::SetWorldViewProj(CXMMATRIX &M)
{
	XMStoreFloat4x4(&m_WorldViewProj, M);
}

void  Render::SetTexTransform(CXMMATRIX &M)
{
	XMStoreFloat4x4(&m_TexTransform, M); 
}

Material Render::GetMaterial()
{
	return m_Material;
}

void  Render::SetMaterial(const Material &mat)
{
	m_Material = mat;
}

void  Render::SetDiffuseMapSRV(ID3D11ShaderResourceView *tex)
{
	m_DiffuseMapSRV = tex;
}

void  Render::SetCubeMapSRV(ID3D11ShaderResourceView *tex)
{
	m_CubeMapSRV = tex;
}

void Render::SetRenderState(ID3D11RasterizerState *pRasterizerState)
{
	m_RenderState = pRasterizerState;
}

void Render::SetBlendState(ID3D11BlendState *pBlendState, const FLOAT BlendFactor[4], UINT SampleMask)
{
	m_BlendState = pBlendState;
	m_BlendFactor = BlendFactor;
	m_SampleMask = SampleMask;
}

void Render::SetDepthStencilState(ID3D11DepthStencilState *pDepthStencilState, UINT StencilRef)
{
	m_DepthStencilState = pDepthStencilState;
	m_StencilRef = StencilRef;
}

// 全局设置
void  Render::SetGlobalDirLights(const DirectionalLight *lights)
{
	Framework::BasicFX->SetDirLights(lights);
}

void  Render::SetEyePosW(const XMFLOAT3 &v)
{
	Framework::BasicFX->SetEyePosW(v);
}

void  Render::SetGlobalFog(const FXMVECTOR &color, float start, float range)
{
	Framework::BasicFX->SetFogColor(color);
	Framework::BasicFX->SetFogStart(start);
	Framework::BasicFX->SetFogRange(range);
}

void Render::SetGlobalRenderState(ID3D11RasterizerState *pRasterizerState)
{
	Framework::Instance().DeviceContext()->RSSetState(pRasterizerState);
}

void Render::SetGlobalBlendState(ID3D11BlendState *pBlendState, const FLOAT BlendFactor[4], UINT SampleMask)
{
	Framework::Instance().DeviceContext()->OMSetBlendState(pBlendState, BlendFactor, SampleMask);
}

void Render::SetGlobalDepthStencilState(ID3D11DepthStencilState *pDepthStencilState, UINT StencilRef)
{
	Framework::Instance().DeviceContext()->OMSetDepthStencilState(pDepthStencilState, StencilRef);
}
