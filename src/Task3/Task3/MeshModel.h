//***************************************************************************************
// MeshModel.h by Linzer Lee
//***************************************************************************************

#ifndef MESHMODEL_H
#define MESHMODEL_H

#include <string>
#include <vector>
#include <d3d11.h>

#include "ModelObject.h"

class Transform;
class Render;
class Collider;

class MeshModel :	public GameObject, 
					public Drawable
{
public:
	MeshModel(std::wstring name);
	~MeshModel();

	DirectX::XMMATRIX World();
	virtual Transform *GetTransform() override;
	virtual Render *GetRender() override;
	virtual Collider *GetCollider() override;

	virtual ModelObject *AddSubModel(UINT *index = nullptr, std::wstring name = L"");
	virtual UINT SubModelSize();
	virtual ModelObject *GetSubModel(UINT index);
	virtual ModelObject *GetSubModel(std::wstring name);
	virtual void DrawSubModel(UINT index, DirectX::FXMMATRIX &viewProj, Camera &cam);

	virtual void Draw(DirectX::FXMMATRIX &viewProj, Camera &cam) override;
	virtual void DrawShadow(DirectX::FXMMATRIX &viewProj, Camera &cam, Material &mat, DirectX::XMFLOAT3 lightDir) override;
	virtual void AddComponent(Component *com) override;
	virtual Component *DetachComponent(std::wstring &name) override;

protected:
	std::vector<ModelObject *> m_ModelSet;
	Transform *m_Transform;
	Render *m_Render;
	Collider *m_Collider;

private:
	MeshModel(const MeshModel& rhs) = delete;
	MeshModel& operator=(const MeshModel& rhs) = delete;
};

#endif // MESHMODEL_H