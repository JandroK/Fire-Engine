#pragma once

#include<vector>
#include<string>

#include "Component.h"

class C_Transform;

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

private:
	std::string name;
	bool active = true;
	bool isStatic = false;
	bool showChildren = false;
	bool toDelete = false;

	GameObject* parent;
	C_Transform* transform;

	std::vector<GameObject*> children;
	std::vector<Component*> components;

};