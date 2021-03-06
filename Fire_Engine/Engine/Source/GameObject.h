#pragma once

#include<vector>
#include<string>

class Component;
enum class ComponentType;
class Transform;

class GameObject
{
public:
	GameObject(const char* name, int UID = -1);
	virtual ~GameObject();

	void Update();

	Component* AddComponent(ComponentType type, int collisionType = 0);
	Component* GetComponent(ComponentType type);

	void Enable();
	void Disable() { active = false; };

	bool GetShowChildrens() { return showChildrens; };
	void SetShowChildrens(bool active) { showChildrens = active; };	

	std::vector<GameObject*> GetChildrens() { return childrens; };
	void AddChildren(GameObject* children) { childrens.push_back(children); };
	void AttachChild(GameObject* child);

	std::vector<GameObject*>::iterator GetBeginChildren() { return childrens.begin(); };
	std::vector<GameObject*>::iterator GetEndChildren() { return childrens.end(); };

	std::vector<GameObject*>::const_iterator FindChildren(GameObject* child) { return std::find(childrens.begin(), childrens.end(), child);};
	GameObject* FindChildren(std::string name);

	void SetIndex(std::vector<GameObject*>::iterator i) { indexList = i; };
	void EraseChildren(std::vector<GameObject*>::const_iterator i) { childrens.erase(i); };

	GameObject* GetParent() { return parent; };
	void SetParent(GameObject* obj) { parent = obj; };
	std::vector<Component*> GetCompoments() { return components; };

	bool IsRoot() { return (parent == nullptr) ? true : false; };
	void Destroy() { pendingToDelete = true;};
	bool GetPendingToDelete() { return pendingToDelete; };
	void SetPendingToDelete(bool isPendingToDelete) {  pendingToDelete= isPendingToDelete; };

	int GetUID() { return UID; };

	Transform* transform = nullptr;

	bool active = true;
	bool isStatic = false;
	bool isSelected = false;

	std::string name;

	std::string tag;
	std::string layer;

private:

	bool showChildrens = false;
	bool pendingToDelete = false;

	std::vector<GameObject*>::iterator indexList;

	GameObject* parent = nullptr;
	std::vector<Component*> components;
	std::vector<GameObject*> childrens;
	int UID = -1;
};