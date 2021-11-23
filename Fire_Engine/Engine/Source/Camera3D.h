#pragma once
#include "Module.h"
#include "Math/float3.h"
#include "ComponentCamera.h"
#include "Geometry/LineSegment.h"

#include "Imgui/imgui.h"
#include "Guizmo/ImGuizmo.h"

class Camera3D : public Module
{
public:
	Camera3D(Application* app, bool start_enabled = true);
	~Camera3D() {};

	void ReStartCamera();
	bool Start() override;

	update_status Update(float dt) override;
	void OnGUI() override;
	void DrawGuizmo(GameObject* obj);
	bool CleanUp() override;

	void CheckInputsKeyBoard();
	void CheckInputsMouse();
	void GenerateRay();
	ImVec2 NormalizeOnWindow(float x, float y, float w, float h, ImVec2 point);


	void LookAt(const float3&Spot);
	void Move(const float3&Movement);

	void OrbitRotation();
	void CalculateViewMatrix();

	float* GetFarPlane() { return &cameraScene.frustrum.farPlaneDistance; }; // Used to change far plane when fog is active

private:
	void Focus();
	void FrontView();

	bool SaveConfig(JsonParser& node)const override;
	bool LoadConfig(JsonParser& node) override;


public:
	
	float3 right, up, front, position, reference;
	float3x3 oldRotation; // Used to reset view rotation

	bool projectionIsDirty = false;

	ComponentCamera cameraScene;
	LineSegment ray;

	ImGuizmo::OPERATION operation;
	ImGuizmo::MODE mode;

	bool rotateSnap = false;
	bool translateSnap = false;
	bool scaleSnap = false;

	float tSnap[3] = { 1.0f, 1.0f, 1.0f };
	float allTsnap = 1.0f;
	int allRsnap = 5;
	float allSsnap = 0.5f;

	float cameraSpeed = 10.0f;
	float zoomSpeed = 50.0f;
	float cameraSensitivity = 0.1f;
};