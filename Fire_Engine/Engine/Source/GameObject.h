#pragma once

#include<vector>
#include<string>

#include "Component.h"

class Transform;

class GameObject
{
public:
	GameObject(const char* name);
	virtual ~GameObject();

	void Update();
	Component* AddComponent(Type type);
	Component* GetComponent(Type type);

	bool isActive() { return active; };
	void Enable();
	void Disable() { active = false; };

	bool IsRoot() { return (parent == nullptr) ? true : false; };
	void Destroy() { toDelete = true;};

	std::vector<GameObject*> GetChildrens() { return children; };
	std::vector<Component*> GetComponents() { return components; };

	GameObject* GetParent() { return parent; };
	Transform* GetTransform() { return transform; };

private:
	std::string name;
	bool active = true;
	bool isStatic = false;
	bool showChildren = false;
	bool toDelete = false;

	GameObject* parent;
	Transform* transform;

	std::vector<GameObject*> children;
	std::vector<Component*> components;

};