//***************************************************************************************
// WavesModel.h by Linzer Lee
//***************************************************************************************

#ifndef WAVESMODEL_H
#define WAVESMODEL_H

#include <string>
#include <d3d11.h>
#include "MathHelper.h"
#include "ModelObject.h"
#include "Waves.h"
#include "Texture.h"
#include "Light.h"

class Camera;

class WavesModel : public ModelObject
{
public:
	WavesModel(std::wstring name);
	virtual ~WavesModel();

	virtual void Draw(DirectX::FXMMATRIX &viewProj, Camera &cam) override;

	void Init(UINT m, UINT n, float dx, float dt, float speed, float damping);
	void Update(float dt, bool isDisturb = false);
	void SetTexture(std::wstring file);
	void SetMaterial(Material mat);
	void AddReflection(Drawable *obj);
	
protected:
	Waves m_Waves;
	Texture m_WavesTex;

private:
	// 供水波计算使用
	DirectX::XMFLOAT2 m_WaterTexOffset;
	DirectX::XMFLOAT4X4 m_TexTransform;
	std::vector<Drawable *> m_Reflections;
};

#endif // WAVESMODEL_H