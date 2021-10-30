#pragma once
#include "Component.h"

class Mesh;

class MeshRenderer : public Component
{
public:
	MeshRenderer(GameObject* obj);
	virtual ~MeshRenderer();

	void Update() override;
	void OnEditor() override;

	void RenderMesh();

	Mesh* GetMesh() { return mesh; };
	void SetMesh(Mesh* mesh) { this->mesh = mesh; };

private:
	Mesh* mesh = nullptr;

	bool faceNormals = false;
	bool vertexNormals = false;
};