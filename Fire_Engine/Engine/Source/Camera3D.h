#pragma once
#include "Module.h"
#include "glmath.h"

#include "Geometry/Frustum.h"

class Camera3D : public Module
{
public:
	Camera3D(Application* app, bool start_enabled = true);
	~Camera3D() {};

	void ReStartCamera();
	bool Start() override;
	update_status Update(float dt) override;
	void CheckInputs();
	bool CleanUp() override;

	void LookAt(const float3&Spot);
	void Look(const float3& Position, const float3& Reference, bool RotateAroundReference = false);
	void Move(const float3&Movement);

	float* GetViewMatrix() { return &ViewMatrix; };

	void OrbitRotation();
	void CalculateViewMatrix();
	void RecalculateProjection();

private:
	void Focus();
	void FrontView();

	bool SaveConfig(JsonParser& node)const override;
	bool LoadConfig(JsonParser& node) override;

public:
	
	float3 X, Y, Z, Position, Reference;

	bool projectionIsDirty = false;
	mat4x4 ViewMatrix;

	Frustum cameraFrustum;

	float aspectRatio = 1.f;
	float verticalFOV = 60.f;
	float nearPlaneDistance = 0.1f;
	float farPlaneDistance = 5000.f;
};