//***************************************************************************************
// Sky.h by Linzer Lee
//***************************************************************************************

#ifndef SKY_H
#define SKY_H

#include <string>
#include <d3d11.h>

#include "ModelObject.h"
#include "Texture.h"
#include "Effects.h"

class Camera;

class Sky : public ModelObject
{
public:
	static const D3D11_INPUT_ELEMENT_DESC InputDesc[1];
	static ID3D11InputLayout* InputLayout;
	static SkyEffect* FX;
	static bool InitClassEnv(ID3D11Device* device);
	static void DeinitClassEnv();

public:
	Sky(std::wstring name);
	~Sky();

	bool Init(const std::wstring& cubemapFilename, const std::wstring& cubeyunmapFilename, float skySphereRadius);
	virtual void Draw(DirectX::FXMMATRIX &viewProj, Camera &cam) override;

	virtual void Update(float dt);

protected:
	Texture m_CubeMapTex;
	Texture m_CubeYunMapTex;
	DirectX::XMFLOAT4X4 m_RotateY;

private:
	Sky(const Sky& rhs) = delete;
	Sky& operator=(const Sky& rhs) = delete;
	// 暂不支持为天空盒子替换组件
	virtual void AddComponent(Component *com) override {};
	virtual Component *DetachComponent(std::wstring &name) override { return nullptr; };
};

#endif // SKY_H