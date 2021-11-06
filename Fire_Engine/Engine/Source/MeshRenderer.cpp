#include "Application.h"
#include "MeshRenderer.h"

#include "Renderer3D.h"
#include "Mesh.h"

#include "GameObject.h"
#include "Transform.h"
#include "Material.h"

#include "ImGui/imgui.h"

MeshRenderer::MeshRenderer(GameObject* obj) : Component(obj)
{
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
		IMGUI_PRINT("Texture coords: ", "%i", mesh->numIndexs);
		IMGUI_PRINT("Indices: ", "%i", mesh->numIndexs);
		// You can enable/disable the mode debug
		ImGui::Checkbox("Vertex Normals", &vertexNormals);
		ImGui::Checkbox("Face Normals", &faceNormals);
	}
}

void MeshRenderer::RenderMesh()
{
	if (active)
	{
		// Push the Matrix to OpenGL
		glPushMatrix();
		glMultMatrixf(GetOwner()->transform->GetGlobalTransformT());

		// Get material component 
		Material* material = dynamic_cast<Material*>(GetOwner()->GetComponent(ComponentType::MATERIAL));
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
	}
}