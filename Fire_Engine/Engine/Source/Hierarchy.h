#pragma once
#include "Tab.h"

class Scene;
class GameObject;

class Hierarchy : public Tab
{
public:
	Hierarchy(Scene* scene);
	virtual ~Hierarchy();

	void Draw() override;
	void SetCurrentScene(Scene* scene) { sceneReference = scene; };

private:
	void DrawGameObjectsTree(GameObject* node, bool drawAsDisabled);
	void RecursiveDrawTree(GameObject* node, bool nodeOpen, bool drawAsDisabled);
	ImGuiTreeNodeFlags SetFlags(GameObject* node);

	Scene* sceneReference;
};
