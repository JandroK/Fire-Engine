#pragma once
#include "Component.h"
#include "Math/float3.h"

class Mesh;

class MeshRenderer : public Component
{
public:
	MeshRenderer(GameObject* obj);
	virtual ~MeshRenderer() { delete mesh; mesh = nullptr; };

	void Update() override;
	void OnEditor() override;

	void RenderMesh();

	Mesh* GetMesh() { return mesh; };
	void SetMesh(Mesh* mesh) { this->mesh = mesh; };

	float3 GetCenterPointInWorldCoords();
	float GetSphereRadius();

private:
	Mesh* mesh = nullptr;

	bool faceNormals = false;
	bool vertexNormals = false;
};