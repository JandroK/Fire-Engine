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
		//if (ImGui::DragFloat3("Linear Factor", &linearFactor.x, 0.05f, 0.f, 1.f) && body)
			//body->setLinearFactor(linearFactor);

		//if (ImGui::DragFloat3("Angular Factor", &angularFactor.x, 0.05f, 0.f, 1.f) && body)
			//body->setAngularFactor(angularFactor);

		ImGui::Checkbox("Use Gravity", &useGravity);
		ImGui::Checkbox("Is Kinematic", &isKinematic);
	}
}
