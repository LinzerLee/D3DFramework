//***************************************************************************************
// Render.h by Linzer Lee
//***************************************************************************************

#ifndef RENDER_H
#define RENDER_H

#include <string>

#include <d3d11.h>

#include "Component.h"
#include "MathHelper.h"
#include "Light.h"

class Render : public Component
{
public:
	COMPONENT_DECLARE(Render)

	Render(std::wstring name);
	virtual ~Render();

	void RenderStateEnable(bool enable);
	void BlendStateEnable(bool enable);
	void DepthStencilStateEnable(bool enable);

	virtual void SetEffectTechnique(const char *tech);
	virtual void SetWorld(DirectX::CXMMATRIX &world);
	virtual void SetWorldViewProj(DirectX::CXMMATRIX &M);
	virtual void Apply(UINT indexCount);

	void SetTexTransform(DirectX::CXMMATRIX &M);
	Material GetMaterial();
	void SetMaterial(const Material &mat);
	void SetDiffuseMapSRV(ID3D11ShaderResourceView *tex);
	void SetCubeMapSRV(ID3D11ShaderResourceView *tex);
	void SetRenderState(ID3D11RasterizerState *pRasterizerState);
	void SetBlendState(ID3D11BlendState *pBlendState, const FLOAT BlendFactor[4], UINT SampleMask = 0xFFFFFFFF);
	void SetDepthStencilState(ID3D11DepthStencilState *pDepthStencilState, UINT StencilRef);
	void SetEyePosW(const DirectX::XMFLOAT3 &v);

	static void SetGlobalFog(const DirectX::FXMVECTOR &color, float start, float range);
	static void SetGlobalDirLights(const DirectionalLight *lights);
	static void SetGlobalRenderState(ID3D11RasterizerState *pRasterizerState);
	static void SetGlobalBlendState(ID3D11BlendState *pBlendState, const FLOAT BlendFactor[4], UINT SampleMask = 0xFFFFFFFF);
	static void SetGlobalDepthStencilState(ID3D11DepthStencilState *pDepthStencilState, UINT StencilRef);

protected:
	bool m_RenderStateEnable;
	bool m_BlendStateEnable;
	bool m_DepthStencilStateEnable;
	std::string m_TechName;
	DirectX::XMFLOAT4X4 m_World;
	DirectX::XMFLOAT4X4 m_WorldViewProj;
	DirectX::XMFLOAT4X4 m_TexTransform;
	Material m_Material;
	ID3D11ShaderResourceView *m_DiffuseMapSRV;	// 不需要负责释放SRV资源
	ID3D11ShaderResourceView * m_CubeMapSRV;    // 不需要负责释放SRV资源
	ID3D11RasterizerState *m_RenderState;	// 不需要负责释放RS资源
	ID3D11BlendState *m_BlendState;				// 不需要负责释放BS资源
	const float *m_BlendFactor;					// 不需要负责释放BF资源
	UINT m_SampleMask;
	ID3D11DepthStencilState *m_DepthStencilState;
	UINT m_StencilRef;
};

#endif // RENDER_H