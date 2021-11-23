#include "Application.h"
#include "Scene.h"
#include "Globals.h"
#include "ResourceMesh.h"

// Components
#include "Transform.h"
#include "MeshRenderer.h"
#include "ComponentCamera.h"

#include "ImGui/imgui.h"
#include "MathGeoLib/include/Math/TransformOps.h"

#include "Guizmo/ImGuizmo.h"
#include "Camera3D.h"

Transform::Transform(GameObject* obj) : Component(obj)
{
	// Inicialize transforms
	globalTransform.SetIdentity();
	localTransform.SetIdentity();

	localTransform.Decompose(position, rotation, scale);
	globalTransform.Decompose(worldPosition, worldRotation, worldScale);

	eulerRotation = rotation.ToEulerXYZ();
	worldEulerRotation = worldRotation.ToEulerXYZ();

	globalTransformTransposed = globalTransform.Transposed();
}

void Transform::Update()
{
}

void Transform::OnEditor()
{
	// New system more efficient:
	// Instead of creating a 4x4 matrix every time a parameter is modified 
	// which is very expensive to calculate because (float4x4 * float4x4 * float4x4) are a lot of GPU cycles,  
	// We only modify the parameter that has been modified, only position or only rotation or only scale.
	// There is more code but it is more efficient
	if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
	{
		CheckStateOperation();

		if(app->camera->mode == ImGuizmo::LOCAL)
			EditTransform(localTransform, position, rotation, eulerRotation, scale);
		else EditTransform(globalTransform, worldPosition, worldRotation, worldEulerRotation, worldScale);

		CheckStateMode();

		ImGui::NewLine();
		// Reset Transform
		if (ImGui::Button("Reset Transform"))
			ResetTransform();

		// If some transfomr has been modify update them
		if (updateTransform)
			UpdateTransform();
	}
}

void Transform::EditTransform(float4x4& trans, float3& pos, Quat& rot, float3& euler, float3& scale)
{
	std::string reference;
	if (app->camera->mode == ImGuizmo::LOCAL)
		reference = "local";
	else reference = "global";

	ImGui::Text("Position %s: ", reference.c_str());
	if (ImGui::DragFloat3("##Position", &pos[0], 0.1f, true))
	{
		// Only overwrite position
		trans.SetCol3(3, pos);
		updateTransform = true;
	}

	ImGui::Text("Rotation %s: ", reference.c_str());
	if (ImGui::DragFloat3("##Rotation", &euler[0], 0.1f, true))
	{
		// We need to do this because otherwise in the inspector the rotation "?" and "?" are "-0" instead of "0" 
		if (euler[0] == 0) euler[0] = 0;
		if (euler[2] == 0) euler[2] = 0;

		// Calculate quaternion
		rot = Quat::FromEulerXYZ(euler.x * DEGTORAD, euler.y * DEGTORAD, euler.z * DEGTORAD);

		// If the scale has not been modified (sum of the diagonal of matrix = 0) then only overwrite rotate
		// But if the scale yes has been modified then float3x3(rotate) * float3x3::Scale(scale)
		if (trans.Trace() == 0)
			trans.SetRotatePart(rot);
		else
			trans.SetRotatePart(float3x3::FromRS(rot, scale));

		updateTransform = true;
	}

	ImGui::Text("Scale %s: ", reference.c_str());
	if (ImGui::DragFloat3("##Scale", &scale[0], 0.1f, true))
	{
		// If the rotation has not been modified (quaternion = identity) then only overwrite scale
		// But if the rotation yes has been modified then float3x3(rotate) * float3x3::Scale(scale)
		if (rot.Equals(Quat::identity))
		{
			trans[0][0] = scale.x;
			trans[1][1] = scale.y;
			trans[2][2] = scale.z;
		}
		else trans.SetRotatePart(float3x3::FromRS(rot, scale));

		updateTransform = true;
	}
}

void Transform::CheckStateMode()
{
	if (app->camera->operation != ImGuizmo::SCALE)
	{
		if (ImGui::RadioButton("Local", app->camera->mode == ImGuizmo::LOCAL))
			app->camera->mode = ImGuizmo::LOCAL;
		ImGui::SameLine();
		if (ImGui::RadioButton("World", app->camera->mode == ImGuizmo::WORLD))
			app->camera->mode = ImGuizmo::WORLD;
	}
}

void Transform::CheckStateOperation()
{
	if (ImGui::IsKeyPressed(26)) //SDL_SCANCODE_W = 26
		app->camera->operation = ImGuizmo::TRANSLATE;
	if (ImGui::IsKeyPressed(8)) //SDL_SCANCODE_E = 8
		app->camera->operation = ImGuizmo::ROTATE;
	if (ImGui::IsKeyPressed(21)) //SDL_SCANCODE_R = 21
		app->camera->operation = ImGuizmo::SCALE;
	//if (ImGui::IsKeyPressed(23)) //SDL_SCANCODE_T = 23
	//	app->camera->operation = ImGuizmo::BOUNDS;

	if (ImGui::RadioButton("Translate", app->camera->operation == ImGuizmo::TRANSLATE))
		app->camera->operation = ImGuizmo::TRANSLATE;
	ImGui::SameLine();
	if (ImGui::RadioButton("Rotate", app->camera->operation == ImGuizmo::ROTATE))
		app->camera->operation = ImGuizmo::ROTATE;
	ImGui::SameLine();
	if (ImGui::RadioButton("Scale", app->camera->operation == ImGuizmo::SCALE))
		app->camera->operation = ImGuizmo::SCALE;
	/*ImGui::SameLine();
	if (ImGui::RadioButton("Bounds", app->camera->operation == ImGuizmo::BOUNDS))
		app->camera->operation = ImGuizmo::BOUNDS;*/
}

// Update globalTransform of children from the component owner
// Example: if the father moves the children too
void Transform::UpdateTransform()
{
	// Store all children transforms which are pending to update
	std::vector<Transform*> transformsToUpdate;
	GetRecursiveTransforms(this, transformsToUpdate);

	// transformsToUpdate.size() = number of children from the component owner  
	for (size_t i = 0; i < transformsToUpdate.size(); i++)
	{
		if (transformsToUpdate[i]->GetOwner()->GetParent() != nullptr)
		{
			Transform* parentTra = transformsToUpdate[i]->GetOwner()->GetParent()->transform;

			if (parentTra != nullptr) {
				// global = global parent * local
				if (app->camera->mode == ImGuizmo::LOCAL)
				{
					transformsToUpdate[i]->globalTransform = parentTra->globalTransform * transformsToUpdate[i]->localTransform;
					transformsToUpdate[i]->globalTransform.Decompose(transformsToUpdate[i]->worldPosition, transformsToUpdate[i]->worldRotation, transformsToUpdate[i]->worldScale);
					transformsToUpdate[i]->worldEulerRotation = transformsToUpdate[i]->worldRotation.ToEulerXYZ() * RADTODEG;
				}
				// local = global parent inverse * global
				else
				{
					if (i == 0)
					{
						localTransform = parentTra->globalTransform.Inverted() * globalTransform;
						localTransform.Decompose(position, rotation, scale);
						eulerRotation = rotation.ToEulerXYZ() * RADTODEG;
					}
					else
					{
						transformsToUpdate[i]->globalTransform = parentTra->globalTransform * transformsToUpdate[i]->localTransform;
						transformsToUpdate[i]->globalTransform.Decompose(transformsToUpdate[i]->worldPosition, transformsToUpdate[i]->worldRotation, transformsToUpdate[i]->worldScale);
						transformsToUpdate[i]->worldEulerRotation = transformsToUpdate[i]->worldRotation.ToEulerXYZ() * RADTODEG;
					}
				}

				transformsToUpdate[i]->globalTransformTransposed = transformsToUpdate[i]->globalTransform.Transposed();

				//Update Bounding Boxes
				transformsToUpdate[i]->UpdateBoundingBoxes();
				ComponentCamera* camera = static_cast<ComponentCamera*>(GetOwner()->GetComponent(ComponentType::CAMERA));
				if (camera != nullptr) camera->updateCamera = true;
			}
		}
	}

	transformsToUpdate.clear();
	updateTransform = false;
}

//Populates an array of childs in descending order
Transform* Transform::GetRecursiveTransforms(Transform* node, std::vector<Transform*>& transforms)
{
	// Store transforms
	transforms.push_back(node);
	int numChildrens = node->GetOwner()->GetChildrens().size();

	// If don't have childrens stop recursive
	if (numChildrens != 0)
	{
		for (int i = 0; i < numChildrens; i++)
		{
			Transform* parentTrans = node->GetOwner()->GetChildrens()[i]->transform;
			GetRecursiveTransforms(parentTrans, transforms);
		}
	}

	return nullptr;
}

void Transform::SetTransformMatrix(float3 position, Quat rotation, float3 localScale, Transform* parent)
{
	this->position = position;
	this->rotation = rotation;
	this->scale = localScale;

	eulerRotation = this->rotation.ToEulerXYZ() * RADTODEG;

	// This is more efficient than localTransform = float4x4::FromTRS()
	localTransform.SetRotatePart(float3x3::FromRS(rotation, scale));
	localTransform.SetCol3(3, position);

	if (parent != nullptr) {
		globalTransform = parent->globalTransform * localTransform;
		globalTransformTransposed = (parent->globalTransform * localTransform).Transposed();
	}
}

void Transform::SetTransformMFromM(float4x4 matrix)
{
	globalTransform = matrix;
	globalTransform.Decompose(worldPosition, worldRotation, worldScale);
	worldEulerRotation = worldRotation.ToEulerXYZ() * RADTODEG;

	localTransform = GetOwner()->GetParent()->transform->globalTransform.Inverted() * globalTransform;
	localTransform.Decompose(position, rotation, scale);
	eulerRotation = rotation.ToEulerXYZ() * RADTODEG;

	globalTransformTransposed = globalTransform.Transposed();

	updateTransform = true;
}

void Transform::NewAttachment()
{
	if (GetOwner()->GetParent() != app->scene->root)
		localTransform = GetOwner()->GetParent()->transform->globalTransform.Inverted().Mul(globalTransform);

	localTransform.Decompose(position, rotation, scale);
	eulerRotation = rotation.ToEulerXYZ();
}

void Transform::ResetTransform()
{
	// Inicialize transforms
	globalTransform.SetIdentity();
	localTransform.SetIdentity();

	localTransform.Decompose(position, rotation, scale);

	eulerRotation = rotation.ToEulerXYZ();

	globalTransformTransposed = globalTransform.Transposed();

	updateTransform = true;
}

void Transform::UpdateBoundingBoxes()
{
	MeshRenderer* mesh = nullptr;
	mesh = static_cast<MeshRenderer*>(GetOwner()->GetComponent(ComponentType::MESHRENDERER));
	if (mesh != nullptr)
	{
		mesh->globalOBB = mesh->GetMesh()->localAABB;
		mesh->globalOBB.Transform(globalTransform);

		mesh->globalAABB.SetNegativeInfinity();
		mesh->globalAABB.Enclose(mesh->globalOBB);
	}	
}

float3 Transform::GetRight()
{
	return GetNormalizeAxis(0);
}
float3 Transform::GetUp()
{
	return GetNormalizeAxis(1);
}
float3 Transform::GetForward()
{
	return GetNormalizeAxis(2);
}

float3 Transform::GetNormalizeAxis(int i)
{
	return globalTransform.RotatePart().Col(i).Normalized();
}