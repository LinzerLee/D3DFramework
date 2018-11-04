//***************************************************************************************
// ParticleSystem.h by Linzer Lee
//***************************************************************************************

#ifndef PARTICLESYSTEM_H
#define PARTICLESYSTEM_H

#include <string>
#include <vector>
#include <d3d11.h>

#include "MathHelper.h"
#include "ModelObject.h"

class Camera;
class ParticleEffect;

class ParticleSystem : public Drawable
{
public:
	struct Vertex
	{
		DirectX::XMFLOAT3 InitialPos;
		DirectX::XMFLOAT3 InitialVel;
		DirectX::XMFLOAT2 Size;
		float Age;
		unsigned int Type;
	};

public:
	static const D3D11_INPUT_ELEMENT_DESC InputDesc[5];
	static ID3D11InputLayout* InputLayout;
	static ParticleEffect* FireFX;
	static ParticleEffect* RainFX;
	static ParticleEffect* SnowFX;
	static bool InitClassEnv(ID3D11Device* device);
	static void DeinitClassEnv();

public:
	ParticleSystem();
	~ParticleSystem();

	// Time elapsed since the system was reset.
	float GetAge()const;

	void SetEmitPos(const DirectX::XMFLOAT3& emitPosW);
	void SetEmitDir(const DirectX::XMFLOAT3& emitDirW);

	void Init(ID3D11Device* device, ParticleEffect* fx, 
		ID3D11ShaderResourceView* texArraySRV, 
		ID3D11ShaderResourceView* randomTexSRV, 
		UINT maxParticles);

	void Reset();
	void Update(float dt, float gameTime);
	void Predraw() override;
	virtual void Draw(DirectX::FXMMATRIX &viewProj, Camera &cam) override;
	virtual void DrawShadow(DirectX::FXMMATRIX &viewProj, Camera &cam, Material &mat, DirectX::XMFLOAT3 lightDir) override {};
	virtual Transform *GetTransform() override { return nullptr; };
	virtual Render *GetRender() override { return nullptr; };
	virtual Collider *GetCollider() override { return nullptr; };

private:
	void BuildVB(ID3D11Device* device);

	ParticleSystem(const ParticleSystem& rhs);
	ParticleSystem& operator=(const ParticleSystem& rhs);
 
private:
 
	UINT m_MaxParticles;
	bool m_FirstRun;

	float m_GameTime;
	float m_TimeStep;
	float m_Age;

	DirectX::XMFLOAT3 m_EmitPosW;
	DirectX::XMFLOAT3 m_EmitDirW;

	ParticleEffect* m_FX;

	ID3D11Buffer* m_InitVB;	
	ID3D11Buffer* m_DrawVB;
	ID3D11Buffer* m_StreamOutVB;
 
	ID3D11ShaderResourceView* m_TexArraySRV;
	ID3D11ShaderResourceView* m_RandomTexSRV;
};

#endif // PARTICLESYSTEM_H