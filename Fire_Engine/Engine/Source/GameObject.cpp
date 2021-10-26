#include "GameObject.h"
#include "Component.h"

GameObject::GameObject(const char* name) : parent(nullptr), name(name)
{
}

GameObject::~GameObject()
{
	for (size_t i = 0; i < components.size(); i++)
	{
		delete components[i];
		components[i] = nullptr;
	}
	components.clear();

	for (size_t i = 0; i < children.size(); i++)
	{
		delete children[i];
		children[i] = nullptr;
	}
	children.clear();
}

void GameObject::Enable()
{
	active = true;

	if (parent != nullptr)
		parent->Enable();
}

void GameObject::Update()
{
	for (size_t i = 0; i < components.size(); i++)
	{
		components[i]->Update();
	}
}

Component* GameObject::AddComponent(Type type)
{
	Component* ret = nullptr;

	switch (type)
	{
	case Type::TRANSFORM:
		break;
	case Type::MESHRENDERER:
		break;
	case Type::MATERIAL:
		break;
	}

	if (ret != nullptr)
	{
		ret->SetType(type);
		components.push_back(ret);
	}

	return ret;
}

Component* GameObject::GetComponent(Type type)
{
	for (size_t i = 0; i < components.size(); i++)
	{
		if (components[i]->GetType() == type)
			return components[i];
	}

	return nullptr;
}