#include <d3d11.h>
#include "Transform.h"

using namespace std;
using namespace DirectX;

COMPONENT_DEFINE(Transform, Transform)

Transform::Transform(wstring name)
:	Component(name),
	m_Offset(0.0f, 0.0f, 0.0f),
	m_Scale(1.0f, 1.0f, 1.0f),
	m_UpdateFlag(false)
{
	XMMATRIX I = XMMatrixIdentity();
	XMStoreFloat4x4(&m_World, I);
	XMStoreFloat4x4(&m_Rotation, I); 
	XMStoreFloat4x4(&m_Attach, I);
}

Transform::~Transform()
{
}
/*
void Transform::SetPosition(DirectX::XMFLOAT3 pos)
{
	m_Position = pos;
	m_UpdateFlag = true;
}
*/
XMFLOAT3 Transform::GetPosition()
{
	XMFLOAT3 pos(0.0f, 0.0f, 0.0f);
	XMVECTOR vpos = XMLoadFloat3(&pos);
	
	vpos = XMVector3TransformCoord(vpos, World());
	XMStoreFloat3(&pos, vpos);

	return pos;
}

void Transform::Translate(XMFLOAT3 offset)
{
	m_Offset = offset;
	m_UpdateFlag = true;
}

void Transform::Translate(FLOAT offsetX, FLOAT offsetY, FLOAT offsetZ)
{
	m_Offset.x = offsetX;
	m_Offset.y = offsetY;
	m_Offset.z = offsetZ;
	m_UpdateFlag = true;
}

void Transform::Translate(FXMVECTOR offset)
{
	XMStoreFloat3(&m_Offset, offset);
	m_UpdateFlag = true;
}

void Transform::Scale(FLOAT scaleX, FLOAT scaleY, FLOAT scaleZ)
{
	m_Scale.x = scaleX; 
	m_Scale.y = scaleY; 
	m_Scale.z = scaleZ; 
	m_UpdateFlag = true;
}

void Transform::Scale(FXMVECTOR scale)
{
	XMStoreFloat3(&m_Scale, scale);
	m_UpdateFlag = true;
}

void Transform::RotateX(FLOAT angle)
{
	XMStoreFloat4x4(&m_Rotation, XMMatrixRotationX(angle));
	m_UpdateFlag = true;
}

void Transform::RotateY(FLOAT angle)
{
	XMStoreFloat4x4(&m_Rotation, XMMatrixRotationY(angle));
	m_UpdateFlag = true;
}

void Transform::RotateZ(FLOAT angle)
{
	XMStoreFloat4x4(&m_Rotation, XMMatrixRotationZ(angle));
	m_UpdateFlag = true;
}

void Transform::RotateAxis(FXMVECTOR axis, FLOAT angle)
{
	XMStoreFloat4x4(&m_Rotation, XMMatrixRotationAxis(axis, angle));
	m_UpdateFlag = true;
}

void Transform::AttachRotateAxis(FXMVECTOR axis, FLOAT angle)
{
	XMMATRIX M = XMLoadFloat4x4(&m_Rotation);
	M = M * XMMatrixRotationAxis(axis, angle);
	XMStoreFloat4x4(&m_Rotation, M);
	m_UpdateFlag = true;
}

void Transform::Attach(CXMMATRIX &M)
{
	XMStoreFloat4x4(&m_Attach, M);
	m_UpdateFlag = true;
}

void Transform::ResetAttach()
{
	XMMATRIX I = XMMatrixIdentity();
	XMStoreFloat4x4(&m_Attach, I);
	m_UpdateFlag = true;
}

XMMATRIX Transform::World()
{
	if (m_UpdateFlag)
	{
		XMFLOAT3 pos = m_Offset;

		XMMATRIX world;

		// 缩放
		world = XMMatrixScaling(m_Scale.x, m_Scale.y, m_Scale.z);
		// 旋转
		world *= XMLoadFloat4x4(&m_Rotation);
		// 平移
		world *= XMMatrixTranslation(pos.x, pos.y, pos.z);
		// 附加变换
		world *= XMLoadFloat4x4(&m_Attach);
		XMStoreFloat4x4(&m_World, world);
	}

	return XMLoadFloat4x4(&m_World);
}
