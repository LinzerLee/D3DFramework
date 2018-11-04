#include "TerrainRender.h"
#include "Framework.h"
#include "Effects.h"
#include "Utils.h"

using namespace DirectX;

COMPONENT_DEFINE(TerrainRender, Render)

TerrainRender::TerrainRender(std::wstring name)
:	Render(name)
{
}

TerrainRender::~TerrainRender()
{
}

void TerrainRender::Apply(UINT count)
{
	ID3DX11EffectTechnique *tech = Terrain::TerrainFX->GetTechniqueByName(m_TechName.c_str());

	if (!tech)
	{
		tech = Terrain::TerrainFX->Light1Tech;
	}

	Framework &fw = Framework::Instance();
	D3DX11_TECHNIQUE_DESC techDesc;

	tech->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		tech->GetPassByIndex(p)->Apply(0, fw.DeviceContext());
		fw.DeviceContext()->DrawIndexed(count, 0, 0);
	}
}

void TerrainRender::SetEyePosW(const XMFLOAT3& v)
{
	Terrain::TerrainFX->SetEyePosW(v);
}

void TerrainRender::SetFogColor(const FXMVECTOR v)
{
	Terrain::TerrainFX->SetFogColor(v);
}

void TerrainRender::SetFogStart(float f)
{
	Terrain::TerrainFX->SetFogStart(f);
}

void TerrainRender::SetFogRange(float f)
{
	Terrain::TerrainFX->SetFogRange(f);
}

void TerrainRender::SetDirLights(const DirectionalLight* lights)
{
	Terrain::TerrainFX->SetDirLights(lights);
}

void TerrainRender::SetMaterial(const Material& mat)
{
	Terrain::TerrainFX->SetMaterial(mat);
}

void TerrainRender::SetMinDist(float f)
{
	Terrain::TerrainFX->SetMinDist(f);
}

void TerrainRender::SetMaxDist(float f)
{
	Terrain::TerrainFX->SetMaxDist(f);
}

void TerrainRender::SetMinTess(float f)
{
	Terrain::TerrainFX->SetMinTess(f);
}

void TerrainRender::SetMaxTess(float f)
{
	Terrain::TerrainFX->SetMaxTess(f);
}

void TerrainRender::SetTexelCellSpaceU(float f)
{
	Terrain::TerrainFX->SetTexelCellSpaceU(f);
}

void TerrainRender::SetTexelCellSpaceV(float f)
{
	Terrain::TerrainFX->SetTexelCellSpaceV(f);
}

void TerrainRender::SetWorldCellSpace(float f)
{
	Terrain::TerrainFX->SetWorldCellSpace(f);
}

void TerrainRender::SetWorldFrustumPlanes(XMFLOAT4 planes[6])
{
	Terrain::TerrainFX->SetWorldFrustumPlanes(planes);
}

void TerrainRender::SetLayerMapArraySRV(ID3D11ShaderResourceView* tex)
{
	Terrain::TerrainFX->SetLayerMapArraySRV(tex);
}

void TerrainRender::SetBlendMapSRV(ID3D11ShaderResourceView* tex)
{
	Terrain::TerrainFX->SetBlendMapSRV(tex);
}

void TerrainRender::SetHeightMapSRV(ID3D11ShaderResourceView* tex)
{
	Terrain::TerrainFX->SetHeightMapSRV(tex);
}