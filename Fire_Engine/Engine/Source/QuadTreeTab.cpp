#include "Application.h"
#include "QuadTreeTab.h"

#include "Camera3D.h"
#include "QuadTreeBase.h"

QuadTreeTab::QuadTreeTab()
{
	name = "Quad Tree";
}

void QuadTreeTab::Draw()
{
	if (ImGui::Begin(name.c_str(), &active))
	{
		ImGui::Text("Static Game Objects: %i", app->camera->quadTree->GetNumGOStatics());
	}
	ImGui::End();
}
