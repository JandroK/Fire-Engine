#include "Transform.h"
#include "Globals.h"

#include "ImGui/imgui.h"

Transform::Transform(GameObject* obj) : Component(obj), updateTransform(false)
{
	globalTransform.SetIdentity();
	localTransform.SetIdentity();

	localTransform.Decompose(position, rotation, localScale);

	eulerRotation = rotation.ToEulerXYZ();

	globalTransformTransposed = globalTransform.Transposed();
}

Transform::~Transform()
{
}

void Transform::Update()
{
}

void Transform::OnEditor()
{
	if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Text("Position: ");
		if (ImGui::DragFloat3("##Pos", &position[0], 0.1f))
			updateTransform = true;

		ImGui::Text("Rotation: ");
		if (ImGui::DragFloat3("##Rot", &eulerRotation[0], 0.1f))
			updateTransform = true;

		ImGui::Text("Scale: ");
		if (ImGui::DragFloat3("##Scale", &localScale[0], 0.1f))
			updateTransform = true;

		if (updateTransform)
			UpdateTransform();
	}
}

void Transform::UpdateTransform()
{
	std::vector<Transform*> transformsToUpdate;
	GetRecursiveTransforms(this, transformsToUpdate);

	rotation = Quat::FromEulerXYZ(eulerRotation.x * DEGTORAD, eulerRotation.y * DEGTORAD, eulerRotation.z * DEGTORAD);
	localTransform = float4x4::FromTRS(position, rotation, localScale);

	//Not working
	if (!transformsToUpdate.empty())
	{
		for (size_t i = 0; i < transformsToUpdate.size(); i++)
		{
			if (transformsToUpdate[i]->GetOwner()->GetParent() != nullptr)
			{
				Transform* parentTra = transformsToUpdate[i]->GetOwner()->GetParent()->GetTransform();

				if (parentTra != nullptr) {
					// = global = global parent * local
					transformsToUpdate[i]->globalTransform = parentTra->globalTransform * transformsToUpdate[i]->localTransform;
					transformsToUpdate[i]->globalTransformTransposed = transformsToUpdate[i]->globalTransform.Transposed();
				}
			}
		}
	}

	transformsToUpdate.clear();
	updateTransform = false;

}

//Populates an array of childs in descending order
Transform* Transform::GetRecursiveTransforms(Transform* node, std::vector<Transform*>& transforms)
{
	transforms.push_back(node);

	if (!node->GetOwner()->GetChildrens().empty())
	{
		for (size_t i = 0; i < node->GetOwner()->GetChildrens().size(); i++)
		{
			Transform* parentTrans = node->GetOwner()->GetChildrens()[i]->GetTransform();
			GetRecursiveTransforms(parentTrans, transforms);
		}
	}

	return nullptr;
}

void Transform::SetTransformMatrix(float3 position, Quat rotation, float3 localScale, Transform* parent)
{
	this->position = position;
	this->rotation = rotation;
	this->localScale = localScale;

	eulerRotation = this->rotation.ToEulerXYZ() * RADTODEG;

	localTransform = float4x4::FromTRS(this->position, this->rotation, this->localScale);

	if (parent != nullptr) {
		globalTransform = parent->globalTransform * localTransform;
		globalTransformTransposed = globalTransform.Transposed();
	}
}