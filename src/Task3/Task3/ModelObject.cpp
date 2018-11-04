#include "ModelObject.h"
#include "Utils.h"
#include "Framework.h"
#include "Transform.h"
#include "Render.h"
#include "Collider.h"
#include "Camera.h"

using namespace std;
using namespace DirectX;

ModelObject::ModelObject(wstring name)
:	GameObject(name),
	m_VB(NULL),
	m_IB(NULL),
	m_IL(NULL),
	m_Topology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST),
	m_IndexCount(0),
	m_Transform(nullptr),
	m_Render(nullptr),
	m_Collider(nullptr)
{
}

ModelObject::~ModelObject()
{
	SafeRelease(m_VB);
	SafeRelease(m_IB);
}

void ModelObject::SetVertexArray(void *array, UINT byteWidth, UINT stride)
{
	assert(array);
	assert(byteWidth != 0);
	if (m_VB)
	{
		SafeRelease(m_VB);
	}

	m_Stride = stride;

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = byteWidth;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = array;
	HR(Framework::Instance().Device()->CreateBuffer(&vbd, &vinitData, &m_VB));
}

void ModelObject::SetIndexArray(void *array, UINT byteWidth, 
	DXGI_FORMAT format, UINT indexCount)
{
	if (m_IB)
	{
		SafeRelease(m_IB);
	}

	m_Format = format;
	m_IndexCount = indexCount;

	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = byteWidth;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = array;
	HR(Framework::Instance().Device()->CreateBuffer(&ibd, &iinitData, &m_IB));
}

void ModelObject::SetVertexBuffer(ID3D11Buffer * vb, UINT stride)
{
	if (m_VB)
	{
		SafeRelease(m_VB);
	}

	m_VB = vb;
	m_Stride = stride;
}

void ModelObject::SetIndexBuffer(ID3D11Buffer* ib, DXGI_FORMAT format, UINT indexCount)
{
	if (m_IB)
	{
		SafeRelease(m_IB);
	}

	m_IB = ib;
	m_Format = format;
	m_IndexCount = indexCount;
}

void ModelObject::SetInputLayout(ID3D11InputLayout *il, D3D11_PRIMITIVE_TOPOLOGY topology)
{
	m_IL = il;
	m_Topology = topology;
}

void ModelObject::Predraw()
{
	UINT offset = 0;
	Framework &fw = Framework::Instance();
	fw.IASetInputLayout(m_IL);
	fw.IASetPrimitiveTopology(m_Topology);
	fw.IASetVertexBuffers(0, 1, &m_VB, &m_Stride, &offset);
	fw.IASetIndexBuffer(m_IB, m_Format, 0);
}

void ModelObject::Draw(FXMMATRIX &viewProj, Camera &cam)
{
	Predraw();

	if (m_Render)
	{
		m_Render->SetWorld(World());
		m_Render->SetEyePosW(cam.GetPosition());
		m_Render->SetWorldViewProj(World() * viewProj);
		m_Render->Apply(m_IndexCount);
	}
	else
	{
		assert(false);
	}
}

void ModelObject::DrawShadow(FXMMATRIX &viewProj, Camera &cam, Material &mat, XMFLOAT3 lightDir)
{
	// 影子
	Material oldMat = m_Render->GetMaterial();
	m_Transform->Attach(XMMatrixShadow(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), -XMLoadFloat3(&lightDir)) * XMMatrixTranslation(0.0f, 0.1f, 0.0f));
	m_Render->SetMaterial(mat);
	m_Render->SetDepthStencilState(Framework::NoDoubleBlendDSS, 0);
	m_Render->DepthStencilStateEnable(false);
	// *绘制雪人影子*
	Draw(viewProj, cam);
	m_Render->DepthStencilStateEnable(true);
	m_Render->SetDepthStencilState(NULL, 0);
	m_Render->SetMaterial(oldMat);
	m_Transform->ResetAttach();
}

XMMATRIX ModelObject::World()
{
	if (m_Transform)
	{
		return m_Transform->World();
	}

	return XMMatrixIdentity();
}

Transform *ModelObject::GetTransform()
{
	return m_Transform;
}

Render *ModelObject::GetRender()
{
	return m_Render;
}

Collider *ModelObject::GetCollider()
{
	BoxCollider *collider = (BoxCollider *)m_Collider;
	collider->UpdateCenter(m_Transform->GetPosition());

	return m_Collider;
}

void ModelObject::UpdateCollider()
{
	if (m_Transform)
	{
		m_Transform->GetPosition();
	}
}

void ModelObject::AddComponent(Component *com)
{
	// Super Call
	GameObject::AddComponent(com);

	string type = com->GetType();
	if (type == Transform::Type)
	{
		m_Transform = (Transform *)com;
	}
	else if(type == Render::Type)
	{
		m_Render = (Render *)com;
	}
	else if (type == Collider::Type)
	{
		m_Collider = (Collider *)com;
	}
	else
	{
		assert(false);
	}
}

Component *ModelObject::DetachComponent(std::wstring &name)
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
				m_Render = nullptr;
			}
		}
		else if (type == Collider::Type)
		{
			if (m_Collider->GetName() == name)
			{
				m_Collider = nullptr;
			}
		}
	}

	return com;
}