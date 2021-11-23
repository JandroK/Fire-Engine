#include "Application.h"
#include "MeshRenderer.h"

#include "Renderer3D.h"
#include "Editor.h"

#include "GameObject.h"
#include "Transform.h"
#include "Material.h"
//#include "Camera3D.h"

#include "ImGui/imgui.h"

MeshRenderer::MeshRenderer(GameObject* obj) : Component(obj)
{
}

MeshRenderer::~MeshRenderer()
{
	delete mesh; mesh = nullptr;
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

void MeshRenderer::RenderMesh()
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