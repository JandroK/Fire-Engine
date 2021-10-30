#include "Application.h"
#include "Material.h"

#include"Renderer3D.h"
#include"ResourceTexture.h"

#include "ImGui/imgui.h"

Material::Material(GameObject* obj) : Component(obj)
{
}

Material::~Material()
{
}

void Material::OnEditor()
{
	if (ImGui::CollapsingHeader("Material", ImGuiTreeNodeFlags_DefaultOpen))
	{
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
	return (viewWithCheckers == false) ? texture->textureID : app->renderer3D->checkersTexture;
}