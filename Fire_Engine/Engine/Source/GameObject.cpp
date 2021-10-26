#include "GameObject.h"
#include "Component.h"

#include "Transform.h"

GameObject::GameObject(const char* name) : parent(nullptr), name(name)
{
	transform = dynamic_cast<Transform*>(AddComponent(ComponentType::TRANSFORM));
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

Component* GameObject::AddComponent(ComponentType type)
{
	assert(type != ComponentType::UNKNOW, "Can't create a UNKNOW component");
	Component* ret = nullptr;

	switch (type)
	{
	case ComponentType::TRANSFORM:
		ret = new Transform(this);
		break;
	case ComponentType::MESHRENDERER:
		break;
	case ComponentType::MATERIAL:
		break;
	}

	if (ret != nullptr)
	{
		ret->SetType(type);
		components.push_back(ret);
	}

	return ret;
}

Component* GameObject::GetComponent(ComponentType type)
{
	for (size_t i = 0; i < components.size(); i++)
	{
		if (components[i]->GetType() == type)
			return components[i];
	}

	return nullptr;
}