#include "C_RigidBody.h"
#include "ImGui/imgui.h"
#include "IconsFontAwesome5.h"
#include "Bullet/include/btBulletDynamicsCommon.h"

C_RigidBody::C_RigidBody(GameObject* obj) : Component(obj)
{
}

C_RigidBody::~C_RigidBody()
{
}

void C_RigidBody::OnEditor()
{
	if (ImGui::CollapsingHeader(ICON_FA_ATOM" Rigid Body", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Checkbox("Active ", &active);

		ImGui::DragFloat("Mass", &mass, 0.1f);

		ImGui::Checkbox("Use Gravity", &useGravity);
		ImGui::Checkbox("Is Kinematic", &isKinematic);

		ImGuiStyle& style = ImGui::GetStyle();
		ImVec4 colors = style.Colors[ImGuiCol_Border];

		ImGui::PushStyleColor(ImGuiCol_Header, IM_COL32(0, 0, 0, 0));
		ImGui::PushStyleColor(ImGuiCol_Border, IM_COL32(0, 0, 0, 0));
		if (ImGui::CollapsingHeader("Constraints"))
		{
			ImGui::PushStyleColor(ImGuiCol_Border, colors);

			ImGui::Text("Freeze Position:");
			if (ImGui::DragFloat3("##FreezePosition", &movementConstraint.x, 0.05f, true, 0.f, 1.f) && body)
				body->setLinearFactor(movementConstraint);

			ImGui::Text("Freeze Rotation:");
			if (ImGui::DragFloat3("##FreezeRotation", &rotationConstraint.x, 0.05f, true, 0.f, 1.f) && body)
				body->setAngularFactor(rotationConstraint);

			ImGui::PopStyleColor();
		}
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
	}
}
