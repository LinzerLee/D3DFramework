//***************************************************************************************
// Camera.h by Linzer Lee
//***************************************************************************************

#ifndef CAMERA_H
#define CAMERA_H

#include "GameObject.h"
#include "MathHelper.h"

class Transform;
class Drawable;

class Camera : public GameObject
{
public:
	Camera();
	~Camera();

	void Attach(Drawable *mod);
	bool AttachMode();

	// Get/Set world camera position.
	DirectX::XMVECTOR GetPositionXM()const;
	DirectX::XMFLOAT3 GetPosition()const;
	void SetPosition(float x, float y, float z);
	void SetPosition(const DirectX::XMFLOAT3& v);

	// Get camera basis vectors.
	DirectX::XMVECTOR GetRightXM()const;
	DirectX::XMFLOAT3 GetRight()const;
	DirectX::XMVECTOR GetUpXM()const;
	DirectX::XMFLOAT3 GetUp()const;
	DirectX::XMVECTOR GetLookXM()const;
	DirectX::XMFLOAT3 GetLook()const;

	// Get frustum properties.
	float GetNearZ()const;
	float GetFarZ()const;
	float GetAspect()const;
	float GetFovY()const;
	float GetFovX()const;

	// Get near and far plane dimensions in view space coordinates.
	float GetNearWindowWidth()const;
	float GetNearWindowHeight()const;
	float GetFarWindowWidth()const;
	float GetFarWindowHeight()const;

	// Set frustum.
	void SetLens(float fovY, float aspect, float zn, float zf);

	// Define camera space via LookAt parameters.
	void LookAt(DirectX::FXMVECTOR pos, DirectX::FXMVECTOR target, DirectX::FXMVECTOR worldUp);
	void LookAt(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& target, const DirectX::XMFLOAT3& up);

	// Get View/Proj matrices.
	DirectX::XMMATRIX View()const;
	DirectX::XMMATRIX Proj()const;
	DirectX::XMMATRIX ViewProj()const;

	// Strafe/Walk the camera a distance d.
	void Strafe(float d);
	void Walk(float d);

	// Rotate the camera.
	void Pitch(float angle);
	void RotateY(float angle);

	// After modifying camera position/orientation, call to rebuild the view matrix.
	void UpdateViewMatrix();

	virtual void AddComponent(Component *com) override;
	virtual Component *DetachComponent(std::wstring &name) override;
	Transform *GetTransform();
protected:
	Drawable * m_Attach;
	Transform * m_Transform;
	// Camera coordinate system with coordinates relative to world space.
	DirectX::XMFLOAT3 m_Position;
	DirectX::XMFLOAT3 m_Right;
	DirectX::XMFLOAT3 m_Up;
	DirectX::XMFLOAT3 m_Look;

	// Cache frustum properties.
	float m_NearZ;
	float m_FarZ;
	float m_Aspect;
	float m_FovY;
	float m_NearWindowHeight;
	float m_FarWindowHeight;

	// Cache View/Proj matrices.
	DirectX::XMFLOAT4X4 m_View;
	DirectX::XMFLOAT4X4 m_Proj;
};

#endif // CAMERA_H