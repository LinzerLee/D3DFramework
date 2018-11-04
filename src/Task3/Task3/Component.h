//***************************************************************************************
// Component.h by Linzer Lee
//***************************************************************************************

#ifndef COMPONENT_H
#define COMPONENT_H

#include <string>
#include <vector>
#include <unordered_map>

#define COMPONENT_DECLARE(__CLASS__)					\
	static const char *Type;							\
	virtual std::string GetType()						\
	{													\
		return std::string(__CLASS__::Type);			\
	}

#define COMPONENT_DEFINE(__CLASS__, __TYPE__)			\
const char *__CLASS__::Type = #__TYPE__;

class Component
{
public:
	Component(std::wstring name);
	virtual ~Component();

	COMPONENT_DECLARE(Component)

	std::wstring GetName();
	Component *GetComponent(std::wstring &name);
	template<class T>
	std::vector<Component *> GetComponent();

	virtual void Initialize() final;
	virtual void Release() final;

protected:
	std::wstring m_Name;
	std::unordered_map<std::wstring, Component *> m_Components;

private:
	unsigned int m_RefCount;
};

template<class T>
std::vector<Component *> Component::GetComponent()
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

#endif // COMPONENT_H