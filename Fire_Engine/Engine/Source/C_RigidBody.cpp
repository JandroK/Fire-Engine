#include "C_RigidBody.h"
#include "Application.h"
#include "Physics3D.h"

#include "ImGui/imgui.h"
#include "IconsFontAwesome5.h"
#include "Bullet/include/btBulletDynamicsCommon.h"

C_RigidBody::C_RigidBody(GameObject* obj) : Component(obj)
{
}

C_RigidBody::~C_RigidBody()
{
	if (body != nullptr)
		app->physics->DeleteBody(body);
}

void C_RigidBody::OnEditor()
{
	// Important condition: body != nullptr 
	// Because OnStop the body don't exist but OnPlay can modify her parameters
	if (ImGui::CollapsingHeader(ICON_FA_ATOM" Rigid Body", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Checkbox("Active    ", &active);
		ImGui::SameLine();
		static const char* collisions[] = { "Box", "Sphere", "Capsule", "Cylinder", "Cone", "Plane" };
		
		ImGui::PushItemWidth(85);
		int currentCollision = (int)collisionType;
		if (ImGui::Combo("Collision Type", &currentCollision, collisions, 6))
			collisionType = (CollisionType)currentCollision;
		ImGui::PopItemWidth();

		ImVec2 winSize = ImGui::GetWindowSize();
		winSize.x -= 130;

		ImGui::Text("Mass:        "); ImGui::SameLine();
		ImGui::PushItemWidth(winSize.x);
		ImGui::DragFloat("##Mass", &mass, 0.1f, 0.1f);
		ImGui::PopItemWidth();

		ImGui::Text("Friction:    "); ImGui::SameLine();
		ImGui::PushItemWidth(winSize.x);
		if (ImGui::DragFloat("##Friction", &friction, 0.05f, 0.f, 1.f) && body)
			body->setFriction(friction);
		ImGui::PopItemWidth();

		ImGui::Text("Restitution: "); ImGui::SameLine();
		ImGui::PushItemWidth(winSize.x);
		if (ImGui::DragFloat("##Restitution", &restitution, 0.1f) && body)
			body->setRestitution(restitution);
		ImGui::PopItemWidth();

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
		if (ImGui::CollapsingHeader("Damping"))
		{
			ImGui::PushStyleColor(ImGuiCol_Border, colors);

			ImGui::Text("Linear Damping:");
			if (ImGui::DragFloat("##LinearDamping", &linearDamping, 0.05f, 0.f, 1.f) && body)
				body->setDamping(linearDamping, angularDamping);

			ImGui::Text("Angular Damping:");
			if (ImGui::DragFloat("##AngularDamping", &angularDamping, 0.05f, 0.f, 1.f) && body)
				body->setDamping(linearDamping, angularDamping);

			ImGui::PopStyleColor();
		}
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
	}
}

void C_RigidBody::CreateBody()
{
	if (body != nullptr)
		app->physics->DeleteBody(body);

	switch (collisionType)
	{
	case CollisionType::BOX:
		body = app->physics->CollisionShape(box, this);
		break;
	case CollisionType::SPHERE:
		body = app->physics->CollisionShape(sphere, this);
		break;
	case CollisionType::CAPSULE:
		body = app->physics->CollisionShape(capsule, this);
		break;
	case CollisionType::CYLINDER:
		body = app->physics->CollisionShape(cylinder, this);
		break;
	case CollisionType::CONE:
		body = app->physics->CollisionShape(cone, this);
		break;
	case CollisionType::STATIC_PLANE:
		body = app->physics->CollisionShape(plane, this);
		break;
	default:
		break;
	}
}
