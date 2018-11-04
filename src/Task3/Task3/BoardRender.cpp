#include "BoardRender.h"
#include "BoardModel.h"
#include "Framework.h"

COMPONENT_DEFINE(BoardRender, Render)

using namespace DirectX;

BoardRender::BoardRender(std::wstring name)
:	Render(name),
	m_FogStart(0.0f),
	m_FogRange(0.0f),
	m_EyePos(0.0f, 0.0f, 0.0f),
	m_FogColor(1.0f, 1.0f, 1.0f, 1.0f),
	m_Lights(NULL),
	m_TexSRV(NULL)
{
	XMMATRIX I = XMMatrixIdentity();
	XMStoreFloat4x4(&m_ViewProj, I);
	
	m_Material.Ambient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	m_Material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	m_Material.Specular = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
}

BoardRender::~BoardRender()
{
}

void BoardRender::Apply(UINT count)
{
	Framework &fw = Framework::Instance();
	ID3DX11EffectTechnique *tech = BoardModel::BoardFX->GetTechniqueByName(m_TechName.c_str());

	if (!tech)
	{
		tech = BoardModel::BoardFX->Light3Tech;
	}

	float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	fw.DeviceContext()->OMSetBlendState(Framework::AlphaToCoverageBS, blendFactor, 0xFFFFFFFF);

	BoardModel::BoardFX->SetDirLights(m_Lights);
	BoardModel::BoardFX->SetViewProj(XMLoadFloat4x4(&m_ViewProj));
	BoardModel::BoardFX->SetEyePosW(m_EyePos);
	BoardModel::BoardFX->SetFogColor(XMLoadFloat4(&m_FogColor));
	BoardModel::BoardFX->SetFogStart(m_FogStart);
	BoardModel::BoardFX->SetFogRange(m_FogRange);
	BoardModel::BoardFX->SetMaterial(m_Material);
	BoardModel::BoardFX->SetTextureMapArray(m_TexSRV);

	D3DX11_TECHNIQUE_DESC techDesc;
	tech->GetDesc(&techDesc);

	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		tech->GetPassByIndex(p)->Apply(0, fw.DeviceContext());
		fw.DeviceContext()->Draw(count, 0);
	}

	fw.DeviceContext()->OMSetBlendState(NULL, blendFactor, 0xFFFFFFFF);
}

void BoardRender::SetViewProj(DirectX::CXMMATRIX M)
{
	XMStoreFloat4x4(&m_ViewProj, M);
}

void BoardRender::SetEyePosW(const DirectX::XMFLOAT3& v)
{
	m_EyePos = v;
}

void BoardRender::SetFog(const DirectX::FXMVECTOR color, float start, float range)
{
	XMStoreFloat4(&m_FogColor, color);
	m_FogStart = start;
	m_FogRange = range;
}

void BoardRender::SetDirLights(const DirectionalLight* lights)
{
	m_Lights = lights;
}

void BoardRender::SetMaterial(const Material& mat)
{
	m_Material = mat;
}

void BoardRender::SetTextureMap(ID3D11ShaderResourceView* tex)
{
	m_TexSRV = tex;
}