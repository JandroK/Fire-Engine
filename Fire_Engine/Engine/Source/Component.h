#pragma once
#include "GameObject.h"

enum class ComponentType
{
	TRANSFORM,
	MESHRENDERER,
	MATERIAL,
};

class Component
{
public:
	Component(GameObject* obj) { owner = obj; };
	virtual ~Component() { owner = nullptr; };

	virtual void Update() {};
	virtual void OnEditor() {};

	ComponentType GetType() { return type; };
	void SetType(ComponentType type) { this->type = type; };

	GameObject* GetOwner() { return owner; };

	bool active = true;
private:
	ComponentType type;
	GameObject* owner = nullptr;
};