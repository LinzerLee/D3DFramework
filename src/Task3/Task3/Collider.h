//***************************************************************************************
// Collider.h by Linzer Lee
//***************************************************************************************

#ifndef COLLIDER_H
#define COLLIDER_H

#include <string>

#include "Component.h"
#include "MathHelper.h"

class Collider : public Component
{
public:
	Collider(std::wstring name);
	virtual ~Collider();

	virtual bool TestPointInside(const DirectX::XMFLOAT3& p) = 0;

	COMPONENT_DECLARE(Collider)
};

class BoxCollider : public Collider
{
public:
	BoxCollider(std::wstring name, DirectX::XMFLOAT3 size);
	virtual ~BoxCollider();

	COMPONENT_DECLARE(BoxCollider)

	void UpdateCenter(DirectX::XMFLOAT3 pos);
	virtual bool TestPointInside(const DirectX::XMFLOAT3& p) override;

protected:
	DirectX::XMFLOAT3 m_Min;
	DirectX::XMFLOAT3 m_Max;
	DirectX::XMFLOAT3 m_Size;
};

#endif // COLLIDER_H
