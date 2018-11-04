//***************************************************************************************
// GameObject.h by Linzer Lee
//***************************************************************************************

#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <string>
#include <vector>
#include <unordered_map>

class Component;

class GameObject
{
public:
	GameObject(std::wstring name);
	virtual ~GameObject();

	void SetTag(std::wstring &tag);
	const std::wstring &GetTag();
	const std::wstring &GetName();
	void AddChild(GameObject *obj);
	GameObject *GetChild(int index);
	int GetChildIndex(GameObject *obj);
	void DetachChild(GameObject *obj);
	GameObject *DetachChild(int index);
	GameObject *GetParent();
	int GetIndex();

	virtual void AddComponent(Component *com);
	virtual Component *DetachComponent(std::wstring &name);

	Component *GetComponent(std::wstring &name);
	template<class T>
	std::vector<Component *> GetComponent();

	static GameObject *Find(std::wstring &name);
	static GameObject *FindWithTag(std::wstring &tag);
	static std::vector<GameObject *> FindGameObjectsWithTag(std::wstring &tag);
	static void Destroy(GameObject *obj);

protected:
	std::wstring m_Name;
	std::wstring m_Tag;
	std::vector<GameObject *> m_Children;
	GameObject *m_Parent;
	std::unordered_map<std::wstring, Component *> m_Components;
	
	static std::unordered_map<std::wstring, GameObject *> m_GameObjects;
	static std::unordered_map<std::wstring, std::vector<GameObject *>> m_TagObjects;

protected:
	virtual void Abstract() {};
};

template<class T>
std::vector<Component *> GameObject::GetComponent()
{
	std::vector<Component *> vc;
	for (auto co : m_Components)
	{
		if (T::Type == co.second->GetType())
		{
			vc.push_back(co.second);
		}
	}

	return vc;
}

#endif // GAMEOBJECT_H