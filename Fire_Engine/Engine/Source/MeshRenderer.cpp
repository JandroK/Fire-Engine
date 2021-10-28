#include "Application.h"
#include "MeshRenderer.h"

#include "Renderer3D.h"
#include "Mesh.h"

#include "GameObject.h"
#include "Transform.h"
#include "Material.h"

#include "ImGui/imgui.h"

MeshRenderer::MeshRenderer(GameObject* obj) : Component(obj), mesh(nullptr)
{
}

MeshRenderer::~MeshRenderer()
{
}

void MeshRenderer::Update()
{
	app->renderer3D->renderQueue.push_back(this);
}

void MeshRenderer::RenderMesh()
{
	Transform* transform = owner->transform;
	if (transform != nullptr)
	{
		glPushMatrix();
		glMultMatrixf(transform->GetGlobalTransposed());
	}

	Material* material = dynamic_cast<Material*>(owner->GetComponent(ComponentType::MATERIAL));
	GLuint id = -1;

	if (material != nullptr)
		id = material->GetTextureID();

	mesh->RenderMesh(-1);

	// If vertexNormals or faceNormals are true draw the Normals
	if (vertexNormals || faceNormals)
		mesh->RenderMeshDebug(&vertexNormals, &faceNormals);

	if (transform != nullptr)
		glPopMatrix();
}

void MeshRenderer::OnEditor()
{
	if (ImGui::CollapsingHeader("Mesh Renderer", ImGuiTreeNodeFlags_DefaultOpen))
	{
		// Print mesh information
		IMGUI_PRINT("Vertices: ", "%i", mesh->numVertex);
		IMGUI_PRINT("Indices: ", "%i", mesh->numIndices);
		IMGUI_PRINT("Texture coords: ", "%i", mesh->numIndices);
		IMGUI_PRINT("Normals: ", "%i", mesh->numNormals);
		// You can enable/disable the mode debug
		ImGui::Checkbox("Vertex Normals", &vertexNormals);
		ImGui::Checkbox("Face Normals", &faceNormals);
	}
}
