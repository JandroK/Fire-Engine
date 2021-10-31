// Not necesarry because #include "Component.h" already includes #include "GameObject.h"
//#include "GameObject.h" 
#include "Component.h"
#include "Globals.h"

// Components
#include "Transform.h"
#include "MeshRenderer.h"
#include "Material.h"

GameObject::GameObject(const char* name) : name(name)
{
	// Each GameObject must have a transform component, that's why we add it when creating it
	transform = dynamic_cast<Transform*>(AddComponent(ComponentType::TRANSFORM));
}

GameObject::~GameObject()
{
	transform = nullptr;
	parent = nullptr;

	// Delete all components
	RELEASE_VECTOR(components, components.size());
	components.clear();

	// Delete all childrens
	RELEASE_VECTOR(childrens, childrens.size());
	childrens.clear();
}

void GameObject::Enable()
{
	active = true;

	// Recursive function, call all fathers
	// Because for the children to be active the father must also be 
	if (parent != nullptr)
		parent->Enable();
}

void GameObject::Update()
{
	// Call all components Updates
	for (size_t i = 0; i < components.size(); i++)
	{
		components[i]->Update();
	}
}
// Add component by Type
Component* GameObject::AddComponent(ComponentType type)
{
	Component* newComponent = nullptr;

	switch (type)
	{
	case ComponentType::TRANSFORM:
		newComponent = new Transform(this);
		break;
	case ComponentType::MESHRENDERER:
		newComponent = new MeshRenderer(this);
		break;
	case ComponentType::MATERIAL:
		newComponent = new Material(this);
		break;
	}

	newComponent->SetType(type);
	components.push_back(newComponent);

	return newComponent;
}
// Get component by Type, because an object can only have 1 component of 1 type
Component* GameObject::GetComponent(ComponentType type)
{
	for (size_t i = 0; i < components.size(); i++)
	{
		if (components[i]->GetType() == type)
			return components[i];
	}

	return nullptr;
}