#include "Application.h"
#include "Material.h"

#include"Renderer3D.h"
#include"ResourceTexture.h"

#include "ImGui/imgui.h"

Material::Material(GameObject* obj) : Component(obj)
{
}

void Material::OnEditor()
{
	if (ImGui::CollapsingHeader("Material", ImGuiTreeNodeFlags_DefaultOpen))
	{
		// This can't call "Active" because there is already another checkbox with that name
		// that's why it's called "Active " 
		ImGui::Checkbox("Active ", &active);
		if (texture->textureID != -1) 
		{
			// Print Texture information, the Image and view model with checkers
			IMGUI_PRINT("Texture Width: ", "%i", texture->texWidth);
			IMGUI_PRINT("Texture Height: ", "%i", texture->texHeight);
			ImGui::Image((ImTextureID)texture->textureID, ImVec2(128, 128));
		}
		ImGui::Checkbox("View with checkers", &viewWithCheckers);
	}
}
// Get Texture ID from the image, this ID is unique
int Material::GetTextureID()
{
	return (viewWithCheckers == false) ? (active == true) ? texture->textureID : -1 : app->renderer3D->checkersTexture;	
}