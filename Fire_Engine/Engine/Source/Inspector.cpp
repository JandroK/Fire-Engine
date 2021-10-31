#include "Application.h"
#include "Inspector.h"

#include "GameObject.h"
#include "Component.h"

#include "ImGui/imgui.h"
#include "ImGui/imgui_internal.h"

Inspector::Inspector() : Tab(), gameObjectSelected(nullptr)
{
	name = "Inspector";

	tags = { "Untagged", "Respawn", "Finish", "EditorOnly", "MainCamara", "Player", "GameController" };
	layers = { "0: Default", "1: TransparentFX", "2: Ignore Raycast", "3: Water", "4: UI", "5: Player" };
}

Inspector::~Inspector()
{
}

void Inspector::Draw()
{
	if (ImGui::Begin(name.c_str()))
	{
		// The inspector is empty if no object is selected 
		if (gameObjectSelected != nullptr)
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
			ImGui::Checkbox("Static", &gameObjectSelected->isStatic);

			DrawList("Tag", &tags);	ImGui::SameLine();	DrawList("Layer", &layers);

			// Destroy object selected, pendingToDelete = true
			if (ImGui::Button("Delete")) {
				gameObjectSelected->Destroy();
			}
			ImGui::Separator();

			// Draw all OnEditors componets
			for (size_t i = 0; i < gameObjectSelected->GetCompomemts().size(); i++)
			{
				gameObjectSelected->GetCompomemts()[i]->OnEditor();
				ImGui::Separator();
			}
		}
	}
	ImGui::End();
}

void Inspector::DrawList(const char* label, std::vector<std::string>* list)
{
	static const char* currentItem = NULL;
	ImGui::Text(label); ImGui::SameLine();

	// Draw the popUp tab of a list and selectable 
	ImGuiStyle& style = ImGui::GetStyle();
	float w = ImGui::CalcItemWidth();
	float spacing = style.ItemInnerSpacing.x;
	float buttonSize = ImGui::GetFrameHeight();
	ImGui::PushItemWidth((w - spacing * 2.0f - buttonSize * 2.0f) * 0.5f);

	std::string listLabel = "##List";
	listLabel.append(label);

	if (ImGui::BeginCombo(listLabel.c_str(), currentItem))
	{
		for (int i = 0; i < list->size(); i++)
		{
			bool isSelected = (currentItem == list->at(i).c_str());
			if (ImGui::Selectable(list->at(i).c_str(), isSelected))
				currentItem = list->at(i).c_str();
			if (isSelected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}
	ImGui::PopItemWidth();	
}