#include "ComponentCamera.h"
#include "ImGui/imgui.h"

ComponentCamera::ComponentCamera(GameObject* obj) : Component(obj)
{
}

void ComponentCamera::Update()
{
}

void ComponentCamera::OnEditor()
{
	if (ImGui::CollapsingHeader("Mesh Renderer", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Checkbox("Show Frustrum Boxes", &showFrustrum);

		if (showFrustrum == true) {
			float3 points[8];
			frustrum.GetCornerPoints(points);
			DrawBoundingBoxes(points, float3(0.2f, 1.f, 0.101f));
		}
	}
}