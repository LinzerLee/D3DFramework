#include "Collider.h"
#include <d3d11.h>

using namespace std;
using namespace DirectX;

COMPONENT_DEFINE(Collider, Collider)
COMPONENT_DEFINE(BoxCollider, Collider)

Collider::Collider(wstring name)
:	Component(name)
{
}

Collider::~Collider()
{
}

BoxCollider::BoxCollider(wstring name, DirectX::XMFLOAT3 size)
:	Collider(name),
	m_Size(size)
{
	m_Min.x = FLT_MAX;
	m_Min.y = FLT_MAX;
	m_Min.z = FLT_MAX;

	m_Max.x = -FLT_MAX;
	m_Max.y = -FLT_MAX;
	m_Max.z = -FLT_MAX;
}

BoxCollider::~BoxCollider()
{
}

void BoxCollider::UpdateCenter(DirectX::XMFLOAT3 pos)
{
	m_Min.x = pos.x - m_Size.x / 2;
	m_Min.y = pos.y - m_Size.y / 2;
	m_Min.z = pos.z - m_Size.z / 2;

	m_Max.x = pos.x + m_Size.x / 2;
	m_Max.y = pos.y + m_Size.y / 2;
	m_Max.z = pos.z + m_Size.z / 2;
}

bool BoxCollider::TestPointInside(const XMFLOAT3& p)
{
	if (p.x >= m_Min.x && p.y >= m_Min.y && p.z >= m_Min.z &&
		p.x <= m_Max.x && p.y <= m_Max.y && p.z <= m_Max.z)
	{
		return true;
	}
	else
	{
		return false;
	}
}