#include "C_RigidBody.h"
#include "Application.h"
#include "Physics3D.h"
#include "Transform.h"
//#include "MeshRenderer.h"

#include "ImGui/imgui.h"
#include "IconsFontAwesome5.h"
#include "Bullet/include/btBulletDynamicsCommon.h"

C_RigidBody::C_RigidBody(GameObject* obj) : Component(obj)
{
	SetBoundingBox();
	CreateBody();
}

C_RigidBody::~C_RigidBody()
{
	if (body != nullptr)
		app->physics->DeleteBody(body);
}

void C_RigidBody::SetBoundingBox()
{
	switch (collisionType)
	{
	case CollisionType::BOX:
		box.transform = GetOwner()->transform->float4x4ToMat4x4();
		break;
	case CollisionType::SPHERE:
		sphere.transform = GetOwner()->transform->float4x4ToMat4x4();
		break;
	//case CollisionType::CAPSULE:
		//capsule.transform = GetOwner()->transform->float4x4ToMat4x4();
		//break;
	case CollisionType::CYLINDER:
		cylinder.transform = GetOwner()->transform->float4x4ToMat4x4();
		break;
	case CollisionType::PYRAMID:
		pyramid.transform = GetOwner()->transform->float4x4ToMat4x4();
		break;
	case CollisionType::STATIC_PLANE:
		plane.transform = GetOwner()->transform->float4x4ToMat4x4();
		break;
	default:
		break;
	}
}

void C_RigidBody::Update()
{
	if (draw) OnDebugDraw();
}

void C_RigidBody::OnEditor()
{
	// Important condition: body != nullptr 
	// Because OnStop the body don't exist but OnPlay can modify her parameters
	if (ImGui::CollapsingHeader(ICON_FA_ATOM" Rigid Body", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Checkbox("Active    ", &active);
		ImGui::SameLine();
		static const char* collisions[] = { "Box", "Sphere", /*"Capsule",*/ "Cylinder", "Cone", "Plane" };
		
		ImGui::PushItemWidth(85);
		int currentCollision = (int)collisionType;
		if (ImGui::Combo("Collision Type", &currentCollision, collisions, 5)) // Change to 6 when add Capsule
		{
			collisionType = (CollisionType)currentCollision;
			SetBoundingBox();
			CreateBody();
		}
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
		if (ImGui::CollapsingHeader("Collision mesh"))
		{
			ImGui::PushStyleColor(ImGuiCol_Border, colors);
			EditCollisionMesh();
			ImGui::PopStyleColor();
		}
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

void C_RigidBody::EditCollisionMesh()
{
	switch (collisionType)
	{
	case CollisionType::BOX:
		ImGui::Text("Box Size: ");
		if (ImGui::DragFloat3("##Box", &box.size.x, 0.1f, true, 0.1f)) editMesh = true;
		break;
	case CollisionType::SPHERE:
		ImGui::Text("Radius: "); ImGui::SameLine();
		if (ImGui::DragFloat("##Radius", &sphere.radius, 0.1f, 0.1f)) editMesh = true;
		break;
	//case CollisionType::CAPSULE:		
		//break;
	case CollisionType::CYLINDER:
		ImGui::Text("Radius: "); ImGui::SameLine();
		if (ImGui::DragFloat("##Radius", &cylinder.radius, 0.1f, 0.1f)) editMesh = true;
		ImGui::Text("Height: "); ImGui::SameLine();
		if (ImGui::DragFloat("##Height", &cylinder.height, 0.1f, 0.1f)) editMesh = true;
		break;
	case CollisionType::PYRAMID:
		ImGui::Text("Radius: "); ImGui::SameLine();
		if (ImGui::DragFloat("##Radius", &pyramid.radius, 0.1f, 0.1f)) editMesh = true;
		ImGui::Text("Height: "); ImGui::SameLine();
		if (ImGui::DragFloat("##Height", &pyramid.height, 0.1f, 0.1f)) editMesh = true;
		break;
	case CollisionType::STATIC_PLANE:
		ImGui::Text("Normal: ");
		if (ImGui::DragFloat3("##Normal", &plane.normal.x, 0.1f, true)) editMesh = true;
		ImGui::Text("Constant: "); ImGui::SameLine();
		if (ImGui::DragFloat("##Constant", &plane.constant, 0.1f)) editMesh = true;
		break;
	default:
		break;
	}

	if (editMesh)
	{
		editMesh = false;
		//CreateBody();
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
	/*case CollisionType::CAPSULE:
		body = app->physics->CollisionShape(capsule, this);
		break;*/
	case CollisionType::CYLINDER:
		body = app->physics->CollisionShape(cylinder, this);
		break;
	case CollisionType::PYRAMID:
		body = app->physics->CollisionShape(pyramid, this);
		break;
	case CollisionType::STATIC_PLANE:
		body = app->physics->CollisionShape(plane, this);
		break;
	default:
		break;
	}

	if (body != nullptr)
	{
		body->setFriction(friction);
		body->setRestitution(restitution);
		body->setLinearFactor(movementConstraint);
		body->setAngularFactor(rotationConstraint);
		body->setDamping(linearDamping, angularDamping);
	}
}

void C_RigidBody::OnDebugDraw() const
{
	switch (collisionType)
	{
	case CollisionType::BOX:
		
		break;
	default:
		break;
	}
}
