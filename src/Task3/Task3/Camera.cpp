#include "Camera.h"
#include "MathHelper.h"
#include "Transform.h"
#include "ModelObject.h"

using namespace std;
using namespace DirectX;

Camera::Camera()
:	GameObject(L"Camera"),
	m_Position(0.0f, 0.0f, 0.0f),
	m_Right(1.0f, 0.0f, 0.0f),
	m_Up(0.0f, 1.0f, 0.0f),
	m_Look(0.0f, 0.0f, 1.0f), 
	m_Transform(nullptr),
	m_Attach(nullptr)
{
	SetLens(0.25f * MathHelper::Pi, 1.0f, 1.0f, 1000.0f);
}

Camera::~Camera()
{
}

void Camera::Attach(Drawable *mod)
{
	if (m_Attach && !mod)
	{
		SetPosition(m_Attach->GetTransform()->GetPosition());
	}

	m_Attach = mod;
}

bool Camera::AttachMode()
{
	return !!m_Attach;
}

XMVECTOR Camera::GetPositionXM()const
{
	return XMLoadFloat3(&m_Position);
}

XMFLOAT3 Camera::GetPosition()const
{
	return m_Position;
}

void Camera::SetPosition(float x, float y, float z)
{
	m_Position = XMFLOAT3(x, y, z);
}

void Camera::SetPosition(const XMFLOAT3& v)
{
	m_Position = v;
}

XMVECTOR Camera::GetRightXM()const
{
	return XMLoadFloat3(&m_Right);
}

XMFLOAT3 Camera::GetRight()const
{
	return m_Right;
}

XMVECTOR Camera::GetUpXM()const
{
	return XMLoadFloat3(&m_Up);
}

XMFLOAT3 Camera::GetUp()const
{
	return m_Up;
}

XMVECTOR Camera::GetLookXM()const
{
	return XMLoadFloat3(&m_Look);
}

XMFLOAT3 Camera::GetLook()const
{
	return m_Look;
}

float Camera::GetNearZ()const
{
	return m_NearZ;
}

float Camera::GetFarZ()const
{
	return m_FarZ;
}

float Camera::GetAspect()const
{
	return m_Aspect;
}

float Camera::GetFovY()const
{
	return m_FovY;
}

float Camera::GetFovX()const
{
	float halfWidth = 0.5f*GetNearWindowWidth();
	return 2.0f*atan(halfWidth / m_NearZ);
}

float Camera::GetNearWindowWidth()const
{
	return m_Aspect * m_NearWindowHeight;
}

float Camera::GetNearWindowHeight()const
{
	return m_NearWindowHeight;
}

float Camera::GetFarWindowWidth()const
{
	return m_Aspect * m_FarWindowHeight;
}

float Camera::GetFarWindowHeight()const
{
	return m_FarWindowHeight;
}

void Camera::SetLens(float fovY, float aspect, float zn, float zf)
{
	// cache properties
	m_FovY = fovY;
	m_Aspect = aspect;
	m_NearZ = zn;
	m_FarZ = zf;

	m_NearWindowHeight = 2.0f * m_NearZ * tanf(0.5f * m_FovY);
	m_FarWindowHeight = 2.0f * m_FarZ * tanf(0.5f * m_FovY);

	XMMATRIX P = XMMatrixPerspectiveFovLH(m_FovY, m_Aspect, m_NearZ, m_FarZ);
	XMStoreFloat4x4(&m_Proj, P);
}

void Camera::LookAt(FXMVECTOR pos, FXMVECTOR target, FXMVECTOR worldUp)
{
	XMVECTOR L = XMVector3Normalize(XMVectorSubtract(target, pos));
	XMVECTOR R = XMVector3Normalize(XMVector3Cross(worldUp, L));
	XMVECTOR U = XMVector3Cross(L, R);

	XMStoreFloat3(&m_Position, pos);
	XMStoreFloat3(&m_Look, L);
	XMStoreFloat3(&m_Right, R);
	XMStoreFloat3(&m_Up, U);
}

void Camera::LookAt(const XMFLOAT3& pos, const XMFLOAT3& target, const XMFLOAT3& up)
{
	XMVECTOR P = XMLoadFloat3(&pos);
	XMVECTOR T = XMLoadFloat3(&target);
	XMVECTOR U = XMLoadFloat3(&up);

	LookAt(P, T, U);
}

XMMATRIX Camera::View()const
{
	return XMLoadFloat4x4(&m_View);
}

XMMATRIX Camera::Proj()const
{
	return XMLoadFloat4x4(&m_Proj);
}

XMMATRIX Camera::ViewProj()const
{
	return XMMatrixMultiply(View(), Proj());
}

void Camera::Strafe(float d)
{
	// m_Position += d * m_Right
	XMVECTOR s = XMVectorReplicate(d);
	XMVECTOR r = XMLoadFloat3(&m_Right);
	XMVECTOR p = XMLoadFloat3(&m_Position);
	XMStoreFloat3(&m_Position, XMVectorMultiplyAdd(s, r, p));
}

void Camera::Walk(float d)
{
	// m_Position += d * m_Look
	XMVECTOR s = XMVectorReplicate(d);
	XMVECTOR l = XMLoadFloat3(&m_Look);
	XMVECTOR p = XMLoadFloat3(&m_Position);
	XMStoreFloat3(&m_Position, XMVectorMultiplyAdd(s, l, p));
}

void Camera::Pitch(float angle)
{
	// Rotate up and look vector about the right vector.

	XMMATRIX R = XMMatrixRotationAxis(XMLoadFloat3(&m_Right), angle);

	XMStoreFloat3(&m_Up, XMVector3TransformNormal(XMLoadFloat3(&m_Up), R));
	XMStoreFloat3(&m_Look, XMVector3TransformNormal(XMLoadFloat3(&m_Look), R));
}

void Camera::RotateY(float angle)
{
	// Rotate the basis vectors about the world y-axis.

	XMMATRIX R = XMMatrixRotationY(angle);

	XMStoreFloat3(&m_Right, XMVector3TransformNormal(XMLoadFloat3(&m_Right), R));
	XMStoreFloat3(&m_Up, XMVector3TransformNormal(XMLoadFloat3(&m_Up), R));
	XMStoreFloat3(&m_Look, XMVector3TransformNormal(XMLoadFloat3(&m_Look), R));
}

void Camera::UpdateViewMatrix()
{
	XMVECTOR R = XMLoadFloat3(&m_Right);
	XMVECTOR U = XMLoadFloat3(&m_Up);
	XMVECTOR L = XMLoadFloat3(&m_Look);
	XMVECTOR P = XMLoadFloat3(&m_Position);

	if (m_Attach && m_Attach->GetTransform())
	{
		XMMATRIX M = m_Attach->GetTransform()->World();
		XMFLOAT4X4 m;
		XMStoreFloat4x4(&m, M);
		P = XMLoadFloat3(&XMFLOAT3(m(3, 0), m(3, 1), m(3, 2)));
	}

	// Keep camera's axes orthogonal to each other and of unit length.
	L = XMVector3Normalize(L);
	U = XMVector3Normalize(XMVector3Cross(L, R));

	// U, L already ortho-normal, so no need to normalize cross product.
	R = XMVector3Cross(U, L);

	// Fill in the view matrix entries.
	float x = -XMVectorGetX(XMVector3Dot(P, R));
	float y = -XMVectorGetX(XMVector3Dot(P, U));
	float z = -XMVectorGetX(XMVector3Dot(P, L));

	XMStoreFloat3(&m_Right, R);
	XMStoreFloat3(&m_Up, U);
	XMStoreFloat3(&m_Look, L);

	m_View(0, 0) = m_Right.x;
	m_View(1, 0) = m_Right.y;
	m_View(2, 0) = m_Right.z;
	m_View(3, 0) = x;

	m_View(0, 1) = m_Up.x;
	m_View(1, 1) = m_Up.y;
	m_View(2, 1) = m_Up.z;
	m_View(3, 1) = y;

	m_View(0, 2) = m_Look.x;
	m_View(1, 2) = m_Look.y;
	m_View(2, 2) = m_Look.z;
	m_View(3, 2) = z;

	m_View(0, 3) = 0.0f;
	m_View(1, 3) = 0.0f;
	m_View(2, 3) = 0.0f;
	m_View(3, 3) = 1.0f;

	if (m_Transform)
	{
		XMStoreFloat4x4(&m_View, m_Transform->World() * XMLoadFloat4x4(&m_View));
	}
}


void Camera::AddComponent(Component *com)
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

Component *Camera::DetachComponent(std::wstring &name)
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
	}

	return com;
}


Transform *Camera::GetTransform()
{
	return m_Transform;
}

