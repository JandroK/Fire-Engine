#include "Application.h"
#include "QuadTreeTab.h"

#include "Camera3D.h"
#include "QuadTreeBase.h"

#include "imgui/imgui_user.h"

QuadTreeTab::QuadTreeTab()
{
	name = "Quad Tree";
}

void QuadTreeTab::Draw()
{
	if (ImGui::Begin(name.c_str(), &active))
	{
		ImGui::Checkbox("Render QuadTree", &app->camera->quadTree->drawQuadTree);
		IMGUI_PRINT("Static Game Objects: ", "%i", app->camera->quadTree->GetNumGOStatics());
		ImGui::NewLine();

		ImGui::PushItemWidth(100);
		ImGui::InputInt("Max. Divisions", &app->camera->quadTree->maxDivisions);
		ImGui::InputInt("Max num GObjects", &app->camera->quadTree->maxGObyNode);
		ImGui::PopItemWidth();

		// If they were unsigned int, this would not be necessary 
		if (app->camera->quadTree->maxDivisions < 1)
			app->camera->quadTree->maxDivisions = 1;
		if (app->camera->quadTree->maxGObyNode < 1)
			app->camera->quadTree->maxGObyNode = 1;

		ImGui::NewLine();
		if (ImGui::Button("Generate QuadTree"))
		{
			app->camera->quadTree->ReCalculateRootLimits();
		}
	}
	ImGui::End();
}
