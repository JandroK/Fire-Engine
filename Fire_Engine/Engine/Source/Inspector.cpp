#include "Application.h"
#include "Inspector.h"
#include "ResourceManager.h"
#include "Camera3D.h"
#include "QuadTreeBase.h"

#include "GameObject.h"
#include "ResourceTexture.h"

#include "ImGui/imgui.h"
#include "ImGui/imgui_internal.h"
#include "IconsFontAwesome5.h"


// Components
#include "Component.h"
#include "Material.h"
#include "MeshRenderer.h"

Inspector::Inspector() : Tab(), gameObjectSelected(nullptr)
{
	name = "Inspector";

	tags = { "Untagged", "Respawn", "Finish", "EditorOnly", "MainCamara", "Player", "GameController" };
	layers = { "Default", "TransparentFX", "Ignore Raycast", "Water", "UI", "Player" };
}

void Inspector::Draw()
{
	if (ImGui::Begin(name.c_str(), &active))
	{
		// The inspector is empty if no object is selected 
		if (gameObjectSelected != nullptr)
		{
			if (item == ItemType::NONE) {

				DrawDefaultInspector();
			}
			else {

				DrawEditLists();		
			}
		}		
	}
	ImGui::End();
}

void Inspector::DrawDefaultInspector()
{
	// If you write "##" the name become on label and this no draw
	if (ImGui::Checkbox("##Active", &gameObjectSelected->active))
	{
		//Warning: The bool has changed at this point, you have to do the reverse
		(gameObjectSelected->active == true) ? gameObjectSelected->Enable() : gameObjectSelected->Disable();
	}
	ImGui::SameLine();

	// Print the name of GameObject and the static checkbox
	char* inputName = &gameObjectSelected->name[0];
	ImGui::InputText("##Name", inputName, gameObjectSelected->name.size() + 1);
	ImGui::SameLine();
	if (ImGui::Checkbox("Static", &gameObjectSelected->isStatic))
		RecursiveSetStaticObjects(gameObjectSelected, gameObjectSelected->isStatic);

	// Draw tagList and layerList
	DrawList("Tag", &tags, gameObjectSelected->tag, maxWidthTag);
	ImGui::SameLine();
	DrawList("Layer", &layers, gameObjectSelected->layer, maxWidthLayers);

	// Destroy object selected, pendingToDelete = true
	if (ImGui::Button(ICON_FA_TRASH" Delete")) {
		gameObjectSelected->Destroy();
	}
	ImGui::Separator();

	// Draw all OnEditors componets
	for (size_t i = 0; i < gameObjectSelected->GetCompoments().size(); i++)
	{
		gameObjectSelected->GetCompoments()[i]->OnEditor();
		ImGui::Separator();
	}
	ImGui::NewLine();

	// Draw Add Component button
	DrawAddComponet();
}

void Inspector::RecursiveSetStaticObjects(GameObject* obj, bool ret)
{
	MeshRenderer* mesh = static_cast<MeshRenderer*>(obj->GetComponent(ComponentType::MESHRENDERER));
	obj->isStatic = ret;
	if (mesh != nullptr)
	{
		if (ret)
			app->camera->quadTree->AddGameObject(obj);
		else
			app->camera->quadTree->RemoveGameObject(obj);
	}
	for (int i = 0; i < obj->GetChildrens().size(); i++)
	{
		RecursiveSetStaticObjects(obj->GetChildrens()[i], ret);
	}

}

void Inspector::DrawAddComponet()
{
	ImGui::PushItemWidth(maxWidthAddC);
	float offset = (ImGui::GetWindowContentRegionMax().x - maxWidthAddC) / 2;
	ImGui::SetCursorPosX(offset);
	if (ImGui::BeginCombo("##AddComponent", "Add Component"))
	{
		// First check if this game object hasn't this component type
		/*if (gameObjectSelected->GetComponent(ComponentType::MESHRENDERER) == nullptr)
		{
		if (ImGui::Selectable("Mesh"))
		gameObjectSelected->AddComponent(ComponentType::MESHRENDERER);
		}*/
		if (gameObjectSelected->GetComponent(ComponentType::MATERIAL) == nullptr)
		{
			if (ImGui::Selectable("Material"))
			{
				Material* mat = static_cast<Material*>(gameObjectSelected->AddComponent(ComponentType::MATERIAL));
				mat->texture = app->resourceManager->defaultTexture;
			}
		}
		if (gameObjectSelected->GetComponent(ComponentType::CAMERA) == nullptr)
		{
			if (ImGui::Selectable("Camera"))
				gameObjectSelected->AddComponent(ComponentType::CAMERA);
		}
		if (gameObjectSelected->GetComponent(ComponentType::RIGIDBODY) == nullptr)
		{
			if (ImGui::Selectable("RigidBody"))
				gameObjectSelected->AddComponent(ComponentType::RIGIDBODY);
		}

		ImGui::EndCombo();
	}
	ImGui::PopItemWidth();
}

void Inspector::DrawEditLists()
{
	if (ImGui::Button(ICON_FA_ARROW_LEFT" Back"))
		item = ItemType::NONE;

	// System to determine which node starts open 
	ImGuiTreeNodeFlags flag = 0;
	if (item == ItemType::TAG) flag = ImGuiTreeNodeFlags_DefaultOpen;
	if (ImGui::CollapsingHeader("Tags", flag))
	{
		DrawListTagLayer("Tag", tags);
	}

	if (item == ItemType::LAYER) flag = ImGuiTreeNodeFlags_DefaultOpen;
	else flag = 0;
	if (ImGui::CollapsingHeader("Layers", flag))
	{
		DrawListTagLayer("Layer", layers);
	}
}

void Inspector::DrawListTagLayer(const char* label, std::vector<std::string> list)
{
	for (int i = 0; i < list.size(); i++)
	{
		ImGui::Text("%s %d: ",label, i);	ImGui::SameLine();
		ImGui::Text(list.at(i).c_str());
	}
	ImGui::Separator();
	ImGui::Text("New %s: ", label);	ImGui::SameLine();

	ImGui::PushItemWidth(100);
	if (label == "Tag")
		ImGui::InputTextWithHint("  ", "Add new Tag", newTag, IM_ARRAYSIZE(newTag));
	else
		ImGui::InputTextWithHint(" ","Add new Layer", newLayer, IM_ARRAYSIZE(newLayer));
	ImGui::PopItemWidth();

	ImGui::SameLine();
	ImGui::PushID(label);
	if (ImGui::Button(ICON_FA_PLUS))
	{
		if (label == "Tag")
		{
			AddTag(newTag);
			newTag[0] = NULL;
		}
		else if (newLayer)
		{
			AddLayer(newLayer);
			newLayer[0] = NULL;
		}
	}
	ImGui::PopID();
	
}

std::string Inspector::DrawList(const char* label, std::vector<std::string>* list, std::string &item, int width)
{
	std::string ret = "";
	ImGui::Text(label); ImGui::SameLine();

	// Set the width of item
	ImGui::PushItemWidth(width);

	std::string listLabel = "##List";
	listLabel.append(label);

	// Draw the popUp tab of a list and the state of current item of in the list has a special style
	if (ImGui::BeginCombo(listLabel.c_str(), item.c_str()))
	{
		for (int i = 0; i < list->size(); i++)
		{
			bool isSelected = !item.compare(list->at(i));
			if (ImGui::Selectable(list->at(i).c_str(), isSelected))
				item = list->at(i);
			if (isSelected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::Separator();

		AddItem(label);

		ImGui::EndCombo();
	}
	ImGui::PopItemWidth();

	return ret;
}

void Inspector::AddItem(const char* label)
{
	std::string addItem = "Add";
	addItem.append(label);
	if (ImGui::Button(addItem.c_str()))
	{
		if (label == "Tag")
			item = ItemType::TAG;
		else if (label == "Layer")
			item = ItemType::LAYER;
	}
}

void Inspector::AddTag(std::string newTag)
{
	if (newTag != "")
	{
		tags.push_back(newTag);
		CalculateMaxWidth(tags, maxWidthTag);
	}	
}

void Inspector::AddLayer(std::string newLayer)
{
	if (newLayer != "")
	{
		layers.push_back(newLayer);
		CalculateMaxWidth(layers, maxWidthLayers);
	}
}

// This function calculate the size of the longest text in a string list in pixels
void Inspector::CalculateMaxWidth(std::vector<std::string> list, int& width)
{
	int w = width;
	for (int i = 0; i < list.size(); i++)
	{
		if (ImGui::CalcTextSize(list.at(i).c_str()).x > width)
			width = ImGui::CalcTextSize(list[i].c_str()).x;
	}

	// Add a margin: GetFrameHeight = button size
	// ItemInnerSpacing.x is the space from the beginning of the container until the first character is drawn 
	// Only if the size has increased
	if (w != width)
	{
		ImGuiStyle& style = ImGui::GetStyle();
		width += ImGui::GetFrameHeight() + style.ItemInnerSpacing.x * 2;
	}	
}
