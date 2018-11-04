//***************************************************************************************
// SkyRender.h by Linzer Lee
//***************************************************************************************

#ifndef SKYRENDER_H
#define SKYRENDER_H

#include <string>

#include "Render.h"

class SkyRender : public Render
{
public:
	COMPONENT_DECLARE(SkyRender)

	SkyRender(std::wstring name);
	virtual ~SkyRender();

	virtual void SetAngle(float angle);
	virtual void SetRotateY(DirectX::CXMMATRIX rotate);
	virtual void Apply(UINT indexCount) override;
	void SetCubeYunMapSRV(ID3D11ShaderResourceView *tex);

protected:
	float m_Angle;
	DirectX::XMFLOAT4X4 m_RotateY;
	ID3D11ShaderResourceView *m_CubeYunMapSRV;
};

#endif // SKYRENDER_H