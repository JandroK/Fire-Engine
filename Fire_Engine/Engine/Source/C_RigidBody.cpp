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


		if (ImGui::CollapsingHeader("Constraints"))
		{
			//if (ImGui::DragFloat3("Freeze position", &movementConstraint.x, 0.05f, 0.f, 1.f) && body)
				//body->setLinearFactor(movementConstraint);

			//if (ImGui::DragFloat3("Freeze rotation", &rotationConstraint.x, 0.05f, 0.f, 1.f) && body)
				//body->setAngularFactor(rotationConstraint);
		}
	}
}
