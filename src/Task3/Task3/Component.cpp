#include "Component.h"

using namespace std;

COMPONENT_DEFINE(Component, Component)

Component::Component(wstring name)
:	m_Name(name),
	m_RefCount(0)
{
}

Component::~Component()
{
	for (auto com : m_Components)
	{
		com.second->Release();
	}
}

void Component::Initialize()
{
	++ m_RefCount;
}

void Component::Release()
{
	if (m_RefCount <= 1)
	{
		delete this;
		return;
	}
	
	-- m_RefCount;
}

Component *Component::GetComponent(std::wstring &name)
{
	Component *com = nullptr;
	auto it = m_Components.find(name);
	if (m_Components.end() != it)
	{
		com = it->second;
	}

	return com;
}

std::wstring Component::GetName()
{
	return m_Name;
}
