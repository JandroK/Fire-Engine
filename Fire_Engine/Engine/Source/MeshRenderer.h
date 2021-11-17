#pragma once
#include "Component.h"
#include "Math/float3.h"
#include "Geometry/AABB.h"
#include "Geometry/OBB.h"

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
	void SetBoundingBoxes(Mesh* mesh);

	float3 GetCenterPointInWorldCoords();
	float GetSphereRadius();

	void DrawBoundingBoxes(float3* points, float3 color = float3::one);

	AABB globalAABB;
	OBB globalOBB;

private:
	Mesh* mesh = nullptr;

	int index[24] = { 0, 2, 2, 6, 6, 4, 4, 0,
					0, 1, 1, 3, 3, 2, 4, 5,
					6, 7, 5, 7, 3, 7, 1, 5 };

	bool faceNormals = false;
	bool vertexNormals = false;
	bool showAABB = false;
	bool showOBB = false;
};