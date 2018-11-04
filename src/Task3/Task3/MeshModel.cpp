#include "MeshModel.h"
#include "Utils.h"
#include "Transform.h"
#include "Render.h"

using namespace std;
using namespace DirectX;

MeshModel::MeshModel(std::wstring name)
:	GameObject(name),
	m_Transform(nullptr),
	m_Render(nullptr),
	m_Collider(nullptr)
{
}

MeshModel::~MeshModel()
{
	for (ModelObject *obj : m_ModelSet)
	{
		SafeDelete(obj);
	}

	SafeRelease(m_Render);
}

XMMATRIX MeshModel::World()
{
	if (m_Transform)
	{
		return m_Transform->World();
	}

	return XMMatrixIdentity();
}

Transform *MeshModel::GetTransform()
{
	return m_Transform;
}

Render *MeshModel::GetRender()
{
	return m_Render;
}

Collider *MeshModel::GetCollider()
{
	return m_Collider;
}

ModelObject *MeshModel::AddSubModel(UINT *index, std::wstring name)
{
	UINT i = 0;
	i = m_ModelSet.size();
	wchar_t buf[32];
	if (L"" == name)
	{
		buf[0] = L'_';
		_itow_s(i, buf + 1, ARRAYSIZE(buf) - 1,10);
		name = m_Name + buf;
	}
	
	ModelObject *obj = new ModelObject(name);
	m_ModelSet.push_back(obj);
	
	if (index)
	{
		*index = i;
	}

	return obj;
}

UINT MeshModel::SubModelSize()
{
	return m_ModelSet.size();
}

ModelObject *MeshModel::GetSubModel(UINT index)
{
	if (index >= 0 && index < m_ModelSet.size())
	{
		return m_ModelSet[index];
	}

	return nullptr;
}

ModelObject *MeshModel::GetSubModel(wstring name)
{
	for (ModelObject *mod : m_ModelSet)
	{
		if (name == mod->GetName())
		{
			return mod;
		}
	}

	return nullptr;
}

void MeshModel::DrawSubModel(UINT index, FXMMATRIX &viewProj, Camera &cam)
{
	ModelObject * obj = GetSubModel(index);
	if (obj)
	{
		obj->Draw(viewProj, cam);
	}
}

void MeshModel::Draw(FXMMATRIX &viewProj, Camera &cam)
{
	for (ModelObject *obj : m_ModelSet)
	{
		obj->Draw(World() * viewProj, cam);
	}
}

void MeshModel::DrawShadow(FXMMATRIX &viewProj, Camera &cam, Material &mat, XMFLOAT3 lightDir)
{
	m_Transform->Attach(XMMatrixShadow(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), -XMLoadFloat3(&lightDir)) * XMMatrixTranslation(0.0f, 0.1f, 0.0f));
	for (ModelObject *obj : m_ModelSet)
	{
		obj->DrawShadow(World() * viewProj, cam, mat, lightDir);
	}
	m_Transform->ResetAttach();
}

void MeshModel::AddComponent(Component *com)
{
	// Super Call
	GameObject::AddComponent(com);

	string type = com->GetType();
	if (type == Transform::Type)
	{
		m_Transform = (Transform *)com;
	}
	else
	{
		assert(false);
	}
}

Component *MeshModel::DetachComponent(std::wstring &name)
{
	// Super Call
	Component *com = GameObject::DetachComponent(name);

	if (com)
	{
		string type = com->GetType();
		if (type == Transform::Type)
		{
			if (m_Transform->GetName() == name)
			{
				m_Transform = nullptr;
			}
		}
		else if (type == Render::Type)
		{
			if (m_Render->GetName() == name)
			{
				m_Transform = nullptr;
			}
		}
	}

	return com;
}
