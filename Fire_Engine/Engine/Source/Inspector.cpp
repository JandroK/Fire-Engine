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

			// Draw tagList and layerList
			DrawList("Tag", &tags, gameObjectSelected->tag, maxWidthTag);
			ImGui::SameLine();	
			DrawList("Layer", &layers, gameObjectSelected->layer, maxWidthLayers);

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
			bool isSelected = item.compare(list->at(i));
			if (ImGui::Selectable(list->at(i).c_str(), isSelected))
				item = list->at(i);
			if (isSelected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}
	ImGui::PopItemWidth();

	return ret;
}

// This function calculate the size of the longest text in a string list in pixels
void Inspector::CalculateMaxWidth(std::vector<std::string> list, int& width)
{
	for (int i = 0; i < list.size(); i++)
	{
		if (ImGui::CalcTextSize(list.at(i).c_str()).x > width)
			width = ImGui::CalcTextSize(list[i].c_str()).x;
	}
	// Add a margin: GetFrameHeight = button size
	// ItemInnerSpacing.x is the space from the beginning of the container until the first character is drawn  
	ImGuiStyle& style = ImGui::GetStyle();
	width += ImGui::GetFrameHeight() + style.ItemInnerSpacing.x * 2;
}