#include "Application.h"
#include "MeshRenderer.h"

#include "Renderer3D.h"
#include "Editor.h"

#include "GameObject.h"
#include "Transform.h"
#include "Material.h"
#include "Scene.h"

#include "ImGui/imgui.h"

MeshRenderer::MeshRenderer(GameObject* obj) : Component(obj)
{
}

MeshRenderer::~MeshRenderer()
{
	// Only delete Mesh if nobody is using it  
	if (!CompareMeshId(app->scene->root, this->GetOwner(), mesh->indexBufferId))
		delete mesh; 
	mesh = nullptr;
}

void MeshRenderer::Update()
{
	app->renderer3D->renderQueue.push_back(this);
}

void MeshRenderer::OnEditor()
{
	if (ImGui::CollapsingHeader("Mesh Renderer", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Checkbox("Active", &active);
		// Print mesh information
		IMGUI_PRINT("Vertices: ", "%i", mesh->numVertex);
		IMGUI_PRINT("Normals: ", "%i", mesh->numNormals);
		IMGUI_PRINT("Tex coords: ", "%i", mesh->numIndexs);
		IMGUI_PRINT("Indices: ", "%i", mesh->numIndexs);

		// You can enable/disable the mode debug
		ImGui::Checkbox("Vertex Normals", &vertexNormals);	ImGui::SameLine();		ImGui::Checkbox("Face Normals", &faceNormals);
		// You can enable/disable the bounding boxes
		ImGui::Checkbox("Show AABB     ", &showAABB);		ImGui::SameLine();		ImGui::Checkbox("Show OBB", &showOBB);
	}
}

void MeshRenderer::DrawMesh()
{
	if (active)
	{
		// Push the Matrix to OpenGL
		glPushMatrix();
		glMultMatrixf(GetOwner()->transform->GetGlobalTransformT().ptr());

		// Get material component 
		Material* material = static_cast<Material*>(GetOwner()->GetComponent(ComponentType::MATERIAL));
		GLuint id = -1;
		// If this gameObject has 
		if (material != nullptr)
			id = material->GetTextureID();

		// Draw
		mesh->Render(id);

		// If vertexNormals or faceNormals are true draw the Normals
		if (vertexNormals || faceNormals)
			mesh->DebugRender(&vertexNormals, &faceNormals);

		// Pop the Matrix to OpenGL
		glPopMatrix();

		DrawSelectedShader();
		
		// If showAABB are enable draw the his bounding boxes
		if (showAABB == true) {
			float3 points[8];
			globalAABB.GetCornerPoints(points);
			DrawBoundingBoxes(points, float3(0.2f, 1.f, 0.101f));
		}
		// If showOBB are enable draw the his bounding boxes
		if (showOBB == true) {
			float3 points[8];
			globalOBB.GetCornerPoints(points);
			DrawBoundingBoxes(points);
		}
	}
}

void MeshRenderer::DrawSelectedShader()
{
	//Transform* outline = new Transform(GetOwner());
	if (app->editor->GetGameObjectSelected() == GetOwner())
	{
		//float3 difference = (app->camera->position - this->GetCenterPointInWorldCoords()).Normalized() * 0.2f;
		float4x4 matrix = GetOwner()->transform->GetGlobalTransform();
		float4x4 reserve = matrix;

		float scaleFactor = 1.01f;
		//Scale is the matrix's diagonal
		matrix[0][0] *= scaleFactor;
		matrix[1][1] *= scaleFactor;
		matrix[2][2] *= scaleFactor;
		//matrix.SetTranslatePart(GetOwner()->transform->GetPosition()/* - difference*/);

		GetOwner()->transform->UpdateTransform();
		GetOwner()->transform->SetTransformMFromM(matrix);

		glPushMatrix();
		glMultMatrixf(GetOwner()->transform->GetGlobalTransformT().ptr());
		glColor4f(1, 0, 0, 0.2f);
		mesh->Render(-1);
		glPopMatrix();
		GetOwner()->transform->SetTransformMFromM(reserve);
		glColor4f(1, 1, 1, 1);
	}
	//RELEASE(outline);
}

void MeshRenderer::SetBoundingBoxes(Mesh* mesh)
{
	// Generate global AABB
	globalOBB = mesh->localAABB;
	globalOBB.Transform(GetOwner()->transform->GetGlobalTransform());

	// Generate global AABB
	globalAABB.SetNegativeInfinity();
	globalAABB.Enclose(globalOBB);
}

float3 MeshRenderer::GetCenterPointInWorldCoords()
{
	return GetOwner()->transform->GetGlobalTransform().TransformPos(mesh->centerPoint);
}

float MeshRenderer::GetSphereRadius()
{
	return mesh->radius;
}

bool MeshRenderer::CompareMeshId(GameObject* node, GameObject* owner, GLuint id)
{
	bool ret = false;
	int numChildrens = node->GetChildrens().size();

	// If don't have childrens stop recursive
	if (numChildrens != 0)
	{
		for (int i = 0; i < numChildrens; i++)
		{
			// Security system: The children that are null we want to check them 
			// Why are NULL? Because we can be in the cleanup process
			for (int j = i; j < numChildrens; j++)
			{
				if (node->GetChildrens()[j] == nullptr) i++;
			}
			if (i == numChildrens) break;

			// Exclude the owner of the id from the check
			if (node->GetChildrens()[i] != owner)
			{
				MeshRenderer* meshR = static_cast<MeshRenderer*>(node->GetChildrens()[i]->GetComponent(ComponentType::MESHRENDERER));
				// Check if this gameObject has material and if so check if the id matches
				if (meshR != nullptr && mesh->indexBufferId == id)
					ret = true;
				// Otherwise check if this gameObject has childrens and the recursion begins
				else if (node->GetChildrens()[i]->GetChildrens().size() != 0)
					ret = CompareMeshId(node->GetChildrens()[i], owner, id);
			}
			if (ret) break;
		}
	}
	return ret;
	return false;
}
