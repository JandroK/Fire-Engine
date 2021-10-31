#include "Application.h"
#include "Hierarchy.h"
#include "Globals.h"

#include "GameObject.h"
#include "Scene.h"
#include "Editor.h"
#include "Inspector.h"
#include "Input.h"

Hierarchy::Hierarchy(Scene* scene) : Tab(), sceneReference(scene)
{
	name = "Hierarchy";
}

Hierarchy::~Hierarchy()
{
	sceneReference = nullptr;
}

void Hierarchy::Draw()
{
	if (ImGui::Begin(name.c_str()))
	{
		// If exist some scene and this have root draw all game object tree
		if (sceneReference != nullptr && sceneReference->root != nullptr)
		{
			DrawGameObjectsTree(sceneReference->root, false);
		}
	}
	ImGui::End();
}

void Hierarchy::DrawGameObjectsTree(GameObject* node, bool drawAsDisabled)
{
	// If this game object is active = false draw on mode disabled
	if (drawAsDisabled == false)
		drawAsDisabled = !node->active;

	ImGuiTreeNodeFlags flags = SetFlags(node);

	// drawAsDisabled = true draw element on diabled mode
	if (drawAsDisabled)
		ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyle().Colors[ImGuiCol_TextDisabled]);

	// This is main function, show gameObject recursively
	// Return false if the arrow is folded, return true if the arrow is unfolded 
	bool nodeOpen = ImGui::TreeNodeEx(node, flags, node->name.c_str());

	if (drawAsDisabled)
		ImGui::PopStyleColor();

	RecursiveDrawTree(node, nodeOpen, drawAsDisabled);
}

void Hierarchy::RecursiveDrawTree(GameObject* node, bool nodeOpen, bool drawAsDisabled)
{
	// Update state of gameObjsect if is selected, execpt the root
	if (ImGui::IsItemClicked() && !node->IsRoot())
	{
		dynamic_cast<Inspector*>(app->editor->GetTab(TabType::INSPECTOR))->gameObjectSelected = node;
	}

	// If it don't has children we will never show them  
	node->SetShowChildrens((node->GetChildrens().size() == 0) ? false : nodeOpen);

	if (node->GetShowChildrens() == true)
	{
		// Call function recursively
		for (unsigned int i = 0; i < node->GetChildrens().size(); i++)
		{
			DrawGameObjectsTree(node->GetChildrens()[i], drawAsDisabled);
		}
		ImGui::TreePop();
	}
}

ImGuiTreeNodeFlags Hierarchy::SetFlags(GameObject* node)
{
	// This flags allow to open the tree if you click on arrow or doubleClick on object, by default the tree is open  
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_DefaultOpen;

	// If GameObject doesn't childrens = no collapsing and no arrow
	if (node->GetChildrens().size() == 0)
		flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

	// If GameObject is selected = activeModeSelected
	if (node == app->editor->GetGameObjectSelected())
		flags |= ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_Selected;

	return flags;
}
