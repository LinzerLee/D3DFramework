#include "SkyRender.h"
#include "Sky.h"
#include "Framework.h"

using namespace std;
using namespace DirectX;

COMPONENT_DEFINE(SkyRender, Render)

SkyRender::SkyRender(wstring name)
:	Render(name),
	m_Angle(0.0f),
	m_CubeYunMapSRV(NULL)
{
	XMMATRIX I = XMMatrixIdentity();
	XMStoreFloat4x4(&m_RotateY, I);
}

SkyRender::~SkyRender()
{
}

void SkyRender::SetAngle(float angle)
{
	m_Angle = angle;
}

void SkyRender::SetRotateY(DirectX::CXMMATRIX rotate)
{
	XMStoreFloat4x4(&m_RotateY, rotate);
}

void SkyRender::SetCubeYunMapSRV(ID3D11ShaderResourceView *tex)
{
	m_CubeYunMapSRV = tex;
}

void SkyRender::Apply(UINT indexCount)
{
	ID3DX11EffectTechnique *tech = Sky::FX->GetTechniqueByName(m_TechName.c_str());

	if (!tech)
	{
		tech = Sky::FX->SkyTech;
	}
	// FX±äÁ¿ÅäÖÃ
	Sky::FX->SetWorldViewProj(XMLoadFloat4x4(&m_WorldViewProj));
	Sky::FX->SetCubeMap(m_CubeMapSRV);
	Sky::FX->SetCubeYunMap(m_CubeYunMapSRV);
	Sky::FX->SetAngle(m_Angle);
	Sky::FX->SetRotateY(XMLoadFloat4x4(&m_RotateY));

	Framework &fw = Framework::Instance();
	D3DX11_TECHNIQUE_DESC techDesc;

	tech->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		tech->GetPassByIndex(p)->Apply(0, fw.DeviceContext());
		fw.DeviceContext()->DrawIndexed(indexCount, 0, 0);
	}
}