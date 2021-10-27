#include "Application.h"
#include "Inspector.h"

#include "GameObject.h"
#include "Component.h"

#include "ImGui/imgui.h"
#include "ImGui/imgui_internal.h"

Inspector::Inspector() : Tab(), gameObjectSelected(nullptr)
{
	name = "Inspector";
}

Inspector::~Inspector()
{
}

void Inspector::Draw()
{
	if (ImGui::Begin(name.c_str()))
	{
		if (gameObjectSelected != nullptr && !gameObjectSelected->IsRoot())
		{
			if (ImGui::Checkbox("##Active", &gameObjectSelected->active))
			{
				//The bool has changed on the checkbox call at this point
				(gameObjectSelected->active == true) ? gameObjectSelected->Enable() : gameObjectSelected->Disable();
			}

			ImGui::SameLine();

			char* inputName = &gameObjectSelected->name[0];
			ImGui::InputText("##Name", inputName, gameObjectSelected->name.size() + 1, ImGuiInputTextFlags_ReadOnly); ImGui::SameLine();
			ImGui::Checkbox("Static", &gameObjectSelected->isStatic);

			ImGui::Text("Tag"); ImGui::SameLine();
			const char* items[] = { "AAAA", "BBBB", "CCCC", "DDDD", "EEEE", "FFFF", "GGGG", "HHHH", "IIII", "JJJJ", "KKKK", "LLLLLLL", "MMMM", "OOOOOOO", "PPPP", "QQQQQQQQQQ", "RRR", "SSSS" };
			static const char* current_item = NULL;

			ImGuiStyle& style = ImGui::GetStyle();
			float w = ImGui::CalcItemWidth();
			float spacing = style.ItemInnerSpacing.x;
			float button_sz = ImGui::GetFrameHeight();
			ImGui::PushItemWidth((w - spacing * 2.0f - button_sz * 2.0f) * 0.5f);
			if (ImGui::BeginCombo("##custom combo", current_item))
			{
				for (int n = 0; n < IM_ARRAYSIZE(items); n++)
				{
					bool is_selected = (current_item == items[n]);
					if (ImGui::Selectable(items[n], is_selected))
						current_item = items[n];
					if (is_selected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}
			ImGui::PopItemWidth();

			ImGui::SameLine();

			ImGui::Text("Layer"); ImGui::SameLine();
			style = ImGui::GetStyle();
			w = ImGui::CalcItemWidth();
			spacing = style.ItemInnerSpacing.x;
			button_sz = ImGui::GetFrameHeight();
			ImGui::PushItemWidth((w - spacing * 2.0f - button_sz * 2.0f) * 0.5f);
			if (ImGui::BeginCombo("##custom combo 2", current_item))
			{
				for (int n = 0; n < IM_ARRAYSIZE(items); n++)
				{
					bool is_selected = (current_item == items[n]);
					if (ImGui::Selectable(items[n], is_selected))
						current_item = items[n];
					if (is_selected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}
			ImGui::PopItemWidth();

			if (ImGui::Button("Delete")) {
				gameObjectSelected->Destroy();
			}

			ImGui::Separator();

			for (size_t i = 0; i < gameObjectSelected->GetComponents().size(); i++)
			{
				gameObjectSelected->GetComponents()[i]->OnEditor();
			}

		}
	}

	ImGui::End();
}