// Not necesarry because #include "Component.h" already includes #include "GameObject.h"
//#include "GameObject.h" 
#include "Component.h"
#include "Globals.h"
#include "Application.h"

// Components
#include "Transform.h"
#include "MeshRenderer.h"
#include "Material.h"
#include "ComponentCamera.h"
#include "C_RigidBody.h"

GameObject::GameObject(const char* name, int _UID) : name(name), tag("Untagged"), layer("0: Default"), UID(_UID)
{
	if(_UID == -1) 
		UID = app->GetNewUID();
	// Each GameObject must have a transform component, that's why we add it when creating it
	transform = static_cast<Transform*>(AddComponent(ComponentType::TRANSFORM));
}

GameObject::~GameObject()
{
	// Delete all childrens
	RELEASE_VECTOR(childrens, childrens.size());
	childrens.clear();

	// Delete all components
	RELEASE_VECTOR(components, components.size());
	components.clear();

	if (pendingToDelete) this->GetParent()->EraseChildren(indexList);

	transform = nullptr;
	parent = nullptr;
}

void GameObject::Enable()
{
	active = true;

	// Recursive function, call all fathers
	// Because for the children to be active the father must also be 
	if (parent != nullptr)
		parent->Enable();
}

void GameObject::AttachChild(GameObject* child)
{
	child->parent = this;
	childrens.push_back(child);
	child->transform->NewAttachment();
	child->transform->UpdateTransform();
}

GameObject* GameObject::FindChildren(std::string name)
{
	for (int i = 0; i < childrens.size(); i++)
	{
		if (childrens.at(i)->name == name)
			return childrens.at(i);
	}
	return nullptr;
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
Component* GameObject::AddComponent(ComponentType type, int collisionType)
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
	case ComponentType::CAMERA:
		newComponent = new ComponentCamera(this);
		break;
	case ComponentType::RIGIDBODY:
		newComponent = new C_RigidBody(this,(CollisionType)collisionType);
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