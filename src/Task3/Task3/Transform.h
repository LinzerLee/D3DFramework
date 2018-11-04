//***************************************************************************************
// Transform.h by Linzer Lee
//***************************************************************************************

#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <string>

#include "Component.h"
#include "MathHelper.h"

class Transform : public Component
{
public:
	Transform(std::wstring name);
	virtual ~Transform();

	COMPONENT_DECLARE(Transform)

	DirectX::XMFLOAT3 GetPosition();
	void Translate(DirectX::XMFLOAT3 offset);
	void Translate(FLOAT offsetX, FLOAT offsetY, FLOAT offsetZ);
	void Translate(DirectX::FXMVECTOR offset);
	void Scale(FLOAT scaleX, FLOAT scaleY, FLOAT scaleZ);
	void Scale(DirectX::FXMVECTOR scale);
	void RotateX(FLOAT angle);
	void RotateY(FLOAT angle);
	void RotateZ(FLOAT angle);
	void RotateAxis(DirectX::FXMVECTOR axis, FLOAT angle);
	void AttachRotateAxis(DirectX::FXMVECTOR axis, FLOAT angle);
	void Attach(DirectX::CXMMATRIX &M);
	void ResetAttach();
	DirectX::XMMATRIX World();

protected:
	// 不用XMMATRIX和XMVECTOR是因为会有16字节对齐问题
	DirectX::XMFLOAT3 m_Offset;
	DirectX::XMFLOAT3 m_Scale;
	DirectX::XMFLOAT4X4 m_Rotation;
	DirectX::XMFLOAT4X4 m_World;
	DirectX::XMFLOAT4X4 m_Attach;
	bool m_UpdateFlag;
};

#endif // TRANSFORM_H