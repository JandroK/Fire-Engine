#pragma once

#include<vector>
#include<string>

class Component;
enum class ComponentType;
class Transform;

class GameObject
{
public:
	GameObject(const char* name);
	virtual ~GameObject();

	void Update();

	Component* AddComponent(ComponentType type);
	Component* GetComponent(ComponentType type);

	void Enable();
	void Disable() { active = false; };

	bool GetShowChildrens() { return showChildrens; };
	void SetShowChildrens(bool active) { showChildrens = active; };

	std::vector<GameObject*> GetChildrens() { return childrens; };
	void AddChildren(GameObject* children) { childrens.push_back(children); };

	GameObject* GetParent() { return parent; };
	void SetParent(GameObject* obj) { parent = obj; };
	std::vector<Component*> GetCompomemts() { return components; };

	bool IsRoot() { return (parent == nullptr) ? true : false; };
	void Destroy() { pendingToDelete = true;};
	bool GetPendingToDelete() { return pendingToDelete; };

	Transform* transform;

	bool active = true;
	bool isStatic = false;
	std::string name;

private:
	GameObject* parent;
	std::vector<GameObject*> childrens;
	std::vector<Component*> components;

	bool showChildrens = false;
	bool pendingToDelete = false;
};