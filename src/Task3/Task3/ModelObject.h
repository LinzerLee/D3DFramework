//***************************************************************************************
// ModelObject.h by Linzer Lee
//***************************************************************************************

#ifndef MODELOBJECT_H
#define MODELOBJECT_H

#include <string>
#include <d3d11.h>
#include "MathHelper.h"
#include "GameObject.h"
#include "Light.h"

class Transform;
class Render;
class Collider;
class Camera;

class Drawable
{
public:
	virtual void Predraw() {};
	virtual void Draw(DirectX::FXMMATRIX &viewProj, Camera &cam) = 0;
	virtual void DrawShadow(DirectX::FXMMATRIX &viewProj, Camera &cam, Material &mat, DirectX::XMFLOAT3 lightDir) = 0;
	virtual Transform *GetTransform() = 0;
	virtual Render *GetRender() = 0;
	virtual Collider *GetCollider() = 0;
};

class ModelObject : public GameObject,
					public Drawable
{
public:
	ModelObject(std::wstring name);
	virtual ~ModelObject();

	void SetVertexArray(void *array, UINT byteWidth, UINT stride);
	void SetIndexArray(void *array, UINT byteWidth, DXGI_FORMAT format, UINT indexCount);
	void SetVertexBuffer(ID3D11Buffer * vb, UINT stride);
	void SetIndexBuffer(ID3D11Buffer* ib, DXGI_FORMAT format, UINT count);
	void SetInputLayout(ID3D11InputLayout *il, D3D11_PRIMITIVE_TOPOLOGY topology);

	DirectX::XMMATRIX World();
	virtual Transform *GetTransform() override;
	virtual Render *GetRender() override;
	virtual Collider *GetCollider() override;
	virtual void UpdateCollider();

	virtual void Predraw() override;
	virtual void Draw(DirectX::FXMMATRIX &viewProj, Camera &cam) override;
	virtual void DrawShadow(DirectX::FXMMATRIX &viewProj, Camera &cam, Material &mat, DirectX::XMFLOAT3 lightDir) override;
	virtual void AddComponent(Component *com) override;
	virtual Component *DetachComponent(std::wstring &name) override;

protected:
	ID3D11Buffer *m_VB;
	ID3D11Buffer *m_IB;
	ID3D11InputLayout *m_IL;
	D3D11_PRIMITIVE_TOPOLOGY m_Topology;
	UINT m_Stride;
	DXGI_FORMAT m_Format;
	UINT m_IndexCount;
	Transform *m_Transform;
	Render *m_Render;
	Collider *m_Collider;

protected:
	// Used For Abstract Base Class
	virtual void Abstract() {};
};

#endif // MODELOBJECT_H