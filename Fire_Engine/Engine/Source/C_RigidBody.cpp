#include "C_RigidBody.h"
#include "Application.h"

#include "Physics3D.h"
#include "Transform.h"
#include "MeshRenderer.h"
#include "Camera3D.h"

#include "ImGui/imgui.h"
#include "IconsFontAwesome5.h"
#include "Bullet/include/btBulletDynamicsCommon.h"

#include "Imgui/imgui_stdlib.h"

C_RigidBody::C_RigidBody(GameObject* obj, float mass, CollisionType type, bool isKinematic) : Component(obj), mass(mass), collisionType(type), isKinematic(isKinematic)
{
	SetCollisionType(type);
	matrix = new btScalar[15];
	// Calculate offset CM
	if (collisionType != CollisionType::CAMERA && static_cast<MeshRenderer*>(GetOwner()->GetComponent(ComponentType::MESHRENDERER)))
	{
		float3 posBody = body->getCenterOfMassPosition();
		float3 posObj = GetOwner()->transform->GetWorldPosition();
		offset = posBody - posObj;
	}	
}

C_RigidBody::C_RigidBody() : Component(nullptr)
{
}

C_RigidBody::~C_RigidBody()
{
	if (body != nullptr)
	{
		if(collisionType == CollisionType::CAMERA)
			app->physics->DeleteBody(this, "Camera");
		else app->physics->DeleteBody(this, GetOwner()->name);

	}
	// Why explode?
	/*
		if (matrix != nullptr) delete[] matrix;
		matrix = nullptr;
	*/
	
}

void C_RigidBody::SetBoundingBox()
{
	float3 pos, radius, size;

	// To collision type Camera or Empty Objects
	if (collisionType == CollisionType::CAMERA)
	{
		pos = app->camera->GetPosition();
	}
	else if (static_cast<MeshRenderer*>(GetOwner()->GetComponent(ComponentType::MESHRENDERER)) == nullptr)
	{
		pos = GetOwner()->transform->GetWorldPosition();
		radius = { 1,1,1 };
		size = { 1,1,1 };
	}
	else
	{
		OBB obb = static_cast<MeshRenderer*>(GetOwner()->GetComponent(ComponentType::MESHRENDERER))->globalOBB;
		pos = obb.CenterPoint();
		radius = obb.r;
		size = obb.Size();
	}		
	
	switch (collisionType)
	{
	case CollisionType::BOX:
		box.FromRS(GetOwner()->transform->GetWorldRotation(), {1,1,1});
		box.SetPos(pos);
		box.size = size;
		break;
	case CollisionType::CAMERA:
		sphere.SetPos(pos);
		sphere.radius = 0.5f;
		break;
	case CollisionType::SPHERE:
		sphere.FromRS(GetOwner()->transform->GetWorldRotation(), { 1,1,1 });
		sphere.SetPos(pos);
		sphere.radius = radius.MaxElement();
		break;
	case CollisionType::CAPSULE:
		capsule.FromRS(GetOwner()->transform->GetWorldRotation(), { 1,1,1 });
		capsule.SetPos(pos);
		capsule.radius = radius.MaxElementXZ();
		capsule.height = size.y;
		break;
	case CollisionType::CYLINDER:
		cylinder.FromRS(GetOwner()->transform->GetWorldRotation(), { 1,1,1 });
		cylinder.SetPos(pos);
		cylinder.radius = radius.MaxElementXZ();
		cylinder.height = size.y;
		break;
	case CollisionType::CONE:
		cone.FromRS(GetOwner()->transform->GetWorldRotation(), { 1,1,1 });
		cone.SetPos(pos);
		cone.radius = radius.MaxElementXZ();
		cone.height = size.y;
		break;
	case CollisionType::STATIC_PLANE:
		plane.FromRS(GetOwner()->transform->GetWorldRotation(), { 1,1,1 });
		plane.SetPos(pos);
		break;
	default:
		break;
	}
}

void C_RigidBody::Update()
{
	if (collisionType == CollisionType::CAMERA)
	{
		btTransform trans;
		body->getMotionState()->getWorldTransform(trans);
		trans.setOrigin(app->camera->GetPosition());
		body->getMotionState()->setWorldTransform(trans);
	}
	else
	{			
		if (body->getActivationState() == 1)
		{
			body->getWorldTransform().getOpenGLMatrix(matrix);
			float4x4 CM = btScalarTofloat4x4(matrix); 
			//float3 f = quatRotate(body->getOrientation(), offset);
			CM.SetCol3(3, CM.Col3(3) - offset);
			GetOwner()->transform->SetTransformMFromM(CM);
		}
	}
}

void C_RigidBody::OnEditor()
{
	// Important condition: body != nullptr 
	// Because OnStop the body don't exist but OnPlay can modify her parameters
	if (ImGui::CollapsingHeader(ICON_FA_ATOM" Rigid Body", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (ImGui::Checkbox("Active    ", &active))
		{
			if (active) app->physics->ActivateCollision(body);
			else app->physics->DesactivateCollision(body);
		}
		ImGui::SameLine();
		static const char* collisions[] = { "Box", "Sphere", "Capsule", "Cylinder", "Cone", "Plane" };
		
		ImGui::PushItemWidth(85);
		int currentCollision = (int)collisionType;
		if (ImGui::Combo("Collision Type", &currentCollision, collisions, 6))
		{
			collisionType = (CollisionType)currentCollision;
			SetCollisionType(collisionType);
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

		Combos();
	}
}

void C_RigidBody::Combos()
{
	ImGuiStyle& style = ImGui::GetStyle();
	ImVec4 colors = style.Colors[ImGuiCol_Border];

	ImGui::PushStyleColor(ImGuiCol_Header, IM_COL32(0, 0, 0, 0));
	ImGui::PushStyleColor(ImGuiCol_Border, IM_COL32(0, 0, 0, 0));
	if (ImGui::CollapsingHeader("Collision mesh"))
	{
		ImGui::PushStyleColor(ImGuiCol_Border, colors);
		EditCollisionMesh();
		ImGui::PopStyleColor();
		ImGui::Separator();
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

		ImGui::NewLine();
		ImGui::Text("Add P2P:"); ImGui::SameLine();
		ImGui::PushItemWidth(150);
		if (ImGui::BeginCombo("##AddConstraint", "Select body"))
		{
			for (int i = 0; i < app->physics->GetBodiesNames().size(); i++)
			{
				btRigidBody* body2 = app->physics->GetBodies().at(i)->body;
				if (body != body2)
				{
					if (ImGui::Selectable(app->physics->GetBodiesNames().at(i).c_str()))
					{
						constraintBodies.push_back(app->physics->GetBodies().at(i));
						app->physics->GetBodies().at(i)->constraintBodies.push_back(this);
						btVector3 center;
						float r1, r2;
						body->getCollisionShape()->getBoundingSphere(center, r1);
						body2->getCollisionShape()->getBoundingSphere(center, r2);
						app->physics->AddConstraintP2P(*body, *body2, (r1, r1, r1), (r2, r2, r2));
					}
				}				
			}
			ImGui::EndCombo();
		}
		ImGui::PopItemWidth();

		ImGui::Text("List of constraint P2P:");
		if (constraintBodies.size() == 0) ImGui::TextColored(ImVec4(1.00f, 1.00f, 1.00f, 0.50f),"-List is empty");
		for (int i = 0; i < constraintBodies.size(); i++)
		{
			ImGui::InputText("##Name", &constraintBodies.at(i)->GetOwner()->name, ImGuiInputTextFlags_::ImGuiInputTextFlags_ReadOnly);
		}

		ImGui::PopStyleColor();
		ImGui::Separator();
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

void C_RigidBody::SetCollisionType(CollisionType type)
{
	collisionType = type;
	SetBoundingBox();
	CreateBody();
	ResetLocalValues();
}

void C_RigidBody::ResetLocalValues()
{
	box.size = { 1,1,1 };
	capsule.radius = 1;
	capsule.height = 2;
	cylinder.radius = 1;
	cylinder.height = 2;
	cone.radius = 1;
	cone.height = 2;
}

void C_RigidBody::EditCollisionMesh()
{
	switch (collisionType)
	{
	case CollisionType::BOX:
		ImGui::Text("Box Size: ");
		if (ImGui::DragFloat3("##Box", &box.size.x, 0.1f, true, 0.1f, INFINITE)) editMesh = true;
		break;
	case CollisionType::SPHERE:
		ImGui::Text("Radius: "); ImGui::SameLine();
		if (ImGui::DragFloat("##Radius", &sphere.radius, 0.1f, 0.1f, INFINITE)) editMesh = true;
		break;
	case CollisionType::CAPSULE:
		ImGui::Text("Radius: "); ImGui::SameLine();
		if (ImGui::DragFloat("##Radius", &capsule.radius, 0.1f, 0.1f, INFINITE)) editMesh = true;
		ImGui::Text("Height: "); ImGui::SameLine();
		if (ImGui::DragFloat("##Height", &capsule.height, 0.1f, 0.1f, INFINITE)) editMesh = true;
		break;
	case CollisionType::CYLINDER:
		ImGui::Text("Radius: "); ImGui::SameLine();
		if (ImGui::DragFloat("##Radius", &cylinder.radius, 0.1f, 0.1f, INFINITE)) editMesh = true;
		ImGui::Text("Height: "); ImGui::SameLine();
		if (ImGui::DragFloat("##Height", &cylinder.height, 0.1f, 0.1f, INFINITE)) editMesh = true;
		break;
	case CollisionType::CONE:
		ImGui::Text("Radius: "); ImGui::SameLine();
		if (ImGui::DragFloat("##Radius", &cone.radius, 0.1f, 0.1f, INFINITE)) editMesh = true;
		ImGui::Text("Height: "); ImGui::SameLine();
		if (ImGui::DragFloat("##Height", &cone.height, 0.1f, 0.1f, INFINITE)) editMesh = true;
		break;
	case CollisionType::STATIC_PLANE:
		ImGui::Text("Normal: ");
		if (ImGui::DragFloat3("##Normal", &plane.normal.x, 0.1f, true, 0.0f, 1))
		{
			if (plane.normal.x == 0.0f && plane.normal.y == 0.0f && plane.normal.z == 0.0f) plane.normal.y = 0.1f;
			editMesh = true;
		}
		ImGui::Text("Constant: "); ImGui::SameLine();
		if (ImGui::DragFloat("##Constant", &plane.constant, 0.1f)) editMesh = true;
		break;
	default:
		break;
	}

	if (editMesh)
	{
		switch (body->getCollisionShape()->getShapeType())
		{
		case BOX_SHAPE_PROXYTYPE:
			static_cast<btBoxShape*>(body->getCollisionShape())->setLocalScaling(box.size);
			break;
		case SPHERE_SHAPE_PROXYTYPE:
			static_cast<btSphereShape*>(body->getCollisionShape())->setUnscaledRadius(sphere.radius);
			break;
		case CAPSULE_SHAPE_PROXYTYPE:
			static_cast<btCapsuleShape*>(body->getCollisionShape())->setLocalScaling(btVector3(capsule.radius, capsule.height * 0.5f, 0.0f));
			break;
		case CYLINDER_SHAPE_PROXYTYPE:
			static_cast<btCylinderShape*>(body->getCollisionShape())->setLocalScaling(btVector3(cylinder.radius, cylinder.height * 0.5f, 0.0f));
			break;
		case CONE_SHAPE_PROXYTYPE:
			static_cast<btConeShape*>(body->getCollisionShape())->setLocalScaling(btVector3(cone.radius, cone.height * 0.5f, cone.radius));
			break;
		case STATIC_PLANE_PROXYTYPE:
			CreateBody();
			break;
		default:
			break;
		}
		editMesh = false;
	}
}

float4x4 C_RigidBody::btScalarTofloat4x4(btScalar* transform)
{
	float4x4 newTransform;
	int k = 0;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			newTransform[j][i] = transform[k];
			k++;
		}
	}

	return newTransform;
}

void C_RigidBody::CreateBody()
{
	if (body != nullptr)
		app->physics->DeleteBody(this, GetOwner()->name);

	switch (collisionType)
	{
	case CollisionType::BOX:
		body = app->physics->CollisionShape(box, this);
		break;
	case CollisionType::CAMERA:
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

	if (body != nullptr)
	{
		body->setFriction(friction);
		body->setRestitution(restitution);
		body->setLinearFactor(movementConstraint);
		body->setAngularFactor(rotationConstraint);
		body->setDamping(linearDamping, angularDamping);
	}
}

void C_RigidBody::SetAsStatic()
{
	useGravity = false;
	isKinematic = false;
	mass = 0.0f;
	SetCollisionType(collisionType);
}
