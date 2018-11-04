//***************************************************************************************
// TerrainRender.h by Linzer Lee
//***************************************************************************************

#ifndef TERRAINRENDER_H
#define TERRAINRENDER_H

#include <string>

#include "Render.h"

class TerrainRender : public Render
{
public:
	COMPONENT_DECLARE(TerrainRender)

	TerrainRender(std::wstring name);
	virtual ~TerrainRender();

	virtual void Apply(UINT count) override;

	void SetEyePosW(const DirectX::XMFLOAT3& v);
	void SetFogColor(const DirectX::FXMVECTOR v);
	void SetFogStart(float f);
	void SetFogRange(float f);
	void SetDirLights(const DirectionalLight* lights);
	void SetMaterial(const Material& mat);
	void SetMinDist(float f);
	void SetMaxDist(float f);
	void SetMinTess(float f);
	void SetMaxTess(float f);
	void SetTexelCellSpaceU(float f);
	void SetTexelCellSpaceV(float f);
	void SetWorldCellSpace(float f);
	void SetWorldFrustumPlanes(DirectX::XMFLOAT4 planes[6]);
	void SetLayerMapArraySRV(ID3D11ShaderResourceView* tex);
	void SetBlendMapSRV(ID3D11ShaderResourceView* tex);
	void SetHeightMapSRV(ID3D11ShaderResourceView* tex);
};

#endif // TERRAINRENDER_H