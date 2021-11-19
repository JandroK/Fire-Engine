#include"Application.h"
#include "GameTab.h"

// Module 
#include "Window.h"
#include "Camera3D.h"

GameTab::GameTab() : Tab()
{
	name = "Game";
}

void GameTab::Draw()
{
	if (ImGui::Begin(name.c_str(), &active, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse))
	{
		// Check if some key/mouseButton are pressed
		if (ImGui::IsWindowHovered())
		{
			app->camera->CheckInputs();
		}
		if (app->camera->GetUpdateAspectRatio() == true) 
			RecalculateAspectRatio();

		// Calculate size of tab scene, get window width and hight and transform the viewport to image and render it 
		ImVec2 texOriginalSize = ImVec2(app->window->GetWindowWidth(), app->window->GetWindowHeight());
		ImVec2 viewportSize = ImGui::GetWindowSize();

		ImVec2 startPoint = ImVec2((texOriginalSize.x / 2) - (viewportSize.x / 2), (texOriginalSize.y / 2) + (viewportSize.y / 2));
		ImVec2 endPoint = ImVec2((texOriginalSize.x / 2) + (viewportSize.x / 2), (texOriginalSize.y / 2) - (viewportSize.y / 2));

		// Normalized coordinates of pixel (10,10) in a 256x256 texture.
		ImVec2 uv0 = ImVec2(startPoint.x / texOriginalSize.x, startPoint.y / texOriginalSize.y);
		// Normalized coordinates of pixel (110,210) in a 256x256 texture.
		ImVec2 uv1 = ImVec2(endPoint.x / texOriginalSize.x, endPoint.y / texOriginalSize.y);

		// Display the 100x200 section starting at (10,10)
		ImGui::Image((ImTextureID)app->camera->cameraScene.texColorBuffer, viewportSize, uv0, uv1);
	}
	ImGui::End();
}

void GameTab::RecalculateAspectRatio()
{
	ImVec2 viewportSize = ImGui::GetWindowSize();

	if (viewportSize.x != lastViewportSize.x || viewportSize.y != lastViewportSize.y)
	{
		app->camera->cameraScene.RecalculateProjection(viewportSize.x / viewportSize.y);
	}
	lastViewportSize = viewportSize;

	app->camera->SetUpdateAspectRatio(false);
}
