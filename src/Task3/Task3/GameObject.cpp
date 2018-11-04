#include "GameObject.h"
#include "Transform.h"
#include "Utils.h"

using namespace std;

unordered_map<wstring, GameObject *> GameObject::m_GameObjects;
unordered_map<wstring, vector<GameObject *>> GameObject::m_TagObjects;

GameObject::GameObject(wstring name)
:	m_Parent(nullptr)
{
	m_Name = name;
}

GameObject::~GameObject()
{
	for (auto obj : m_GameObjects)
	{
		SafeDelete(obj.second);
	}

	m_GameObjects.clear();

	for (auto com : m_Components)
	{
		SafeRelease(com.second);
	}

	m_Components.clear();
}

void GameObject::SetTag(wstring &tag)
{
	// 检测与原有的tag是否相同
	if (m_Tag != tag)
	{
		auto it = m_TagObjects.find(tag);
		if (m_TagObjects.end() != it)
		{
			for (auto obj = it->second.begin(); obj != it->second.end(); ++obj)
			{
				if (this == *obj)
				{
					it->second.erase(obj);
					break;
				}
			}

			// 没有对象就从字典中删除
			if (it->second.size() <= 0)
			{
				m_TagObjects.erase(tag);
			}
		}
		m_Tag = tag;
		m_TagObjects[m_Tag].push_back(this);
	}
}

const wstring &GameObject::GetTag()
{
	return m_Tag;
}

const wstring &GameObject::GetName()
{
	return m_Name;
}

void GameObject::AddChild(GameObject *obj)
{
	for (auto o : m_Children)
	{
		if (o == obj)
		{
			return;
		}
	}

	m_Children.push_back(obj);
	obj->m_Parent = this;
}

GameObject *GameObject::GetChild(int index)
{
	if (index >= 0 && index < (int)m_Children.size())
	{
		return m_Children[index];
	}

	return nullptr;
}

int GameObject::GetChildIndex(GameObject *obj)
{
	for (auto it = m_Children.begin(); it != m_Children.end(); ++it)
	{
		if (*it == obj)
		{
			return it - m_Children.begin();
		}
	}

	return -1;
}

void GameObject::DetachChild(GameObject *obj)
{
	int index = GetChildIndex(obj);
	if (-1 != index)
	{
		DetachChild(index);
	}
}

GameObject *GameObject::DetachChild(int index)
{
	if (index >= 0 && index < (int)m_Children.size())
	{
		GameObject *obj = m_Children[index];
		m_Children.erase(m_Children.begin() + index);
		return obj;
	}

	return nullptr;
}

GameObject *GameObject::GetParent()
{
	return m_Parent;
}

int GameObject::GetIndex()
{
	if (m_Parent)
	{
		return m_Parent->GetChildIndex(this);
	}

	return -1;
}

void GameObject::AddComponent(Component *com)
{
	m_Components[com->GetName()] = com;
	SafeInitialize(com);
}

Component *GameObject::DetachComponent(std::wstring &name)
{
	Component *com = nullptr;
	auto it = m_Components.find(name);
	if (it != m_Components.end())
	{
		com = it->second;
		m_Components.erase(it);
		SafeRelease(com);
	}
	
	return com;
}

Component *GameObject::GetComponent(std::wstring &name)
{
	Component *com = nullptr;
	auto it = m_Components.find(name);
	if (m_Components.end() != it)
	{
		com = it->second;
	}

	return com;
}

GameObject *GameObject::Find(wstring &name)
{
	GameObject *obj = nullptr;
	auto it = m_GameObjects.find(name);
	if (m_GameObjects.end() != it)
	{
		obj = it->second;
	}

	return obj;
}

GameObject *GameObject::FindWithTag(std::wstring &tag)
{
	GameObject *obj = nullptr;
	auto it = m_TagObjects.find(tag);
	if (m_TagObjects.end() != it)
	{
		obj = it->second[0];
	}

	return obj;
}

vector<GameObject *> GameObject::FindGameObjectsWithTag(wstring &tag)
{
	auto it = m_TagObjects.find(tag);
	if (m_TagObjects.end() != it)
	{
		return it->second;
	}

	return vector<GameObject *>();
}

void GameObject::Destroy(GameObject *obj)
{
	if (obj)
	{
		GameObject *parent = obj->GetParent();
		if (parent)
		{
			parent->DetachChild(obj);
		}
	}
}