#pragma once

class GameObject;
enum class Type
{
	UNKNOW,
	TRANSFORM,
	MESHRENDERER,
	MATERIAL
};

class Component
{
public:
	Component(GameObject* obj) { owner = obj; };
	virtual ~Component() {};

	virtual void Enable() {};
	virtual void Disable() {};

	virtual void Update() {};
	virtual void OnEditor() {};

	Type GetType() { return type; };
	void SetType(Type type) { this->type = type; };

private:
	Type type = Type::UNKNOW;
	bool active = true;
	GameObject* owner;

};