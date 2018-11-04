//***************************************************************************************
// BoardRender.h by Linzer Lee
//***************************************************************************************

#ifndef BOARDRENDER_H
#define BOARDRENDER_H

#include <string>

#include "Render.h"

class BoardRender : public Render
{
public:
	COMPONENT_DECLARE(BoardRender)

	BoardRender(std::wstring name);
	virtual ~BoardRender();

	virtual void Apply(UINT count) override;

	void SetViewProj(DirectX::CXMMATRIX M);
	void SetEyePosW(const DirectX::XMFLOAT3& v);
	void SetFog(const DirectX::FXMVECTOR color, float start, float range);
	void SetDirLights(const DirectionalLight* lights);
	void SetMaterial(const Material& mat);
	void SetTextureMap(ID3D11ShaderResourceView* tex);

protected:
	DirectX::XMFLOAT3 m_EyePos;
	DirectX::XMFLOAT4X4 m_ViewProj;
	DirectX::XMFLOAT4 m_FogColor;
	FLOAT m_FogStart;
	FLOAT m_FogRange;
	const DirectionalLight *m_Lights;
	Material  m_Material;
	ID3D11ShaderResourceView *m_TexSRV;
};

#endif // BOARDRENDER_H