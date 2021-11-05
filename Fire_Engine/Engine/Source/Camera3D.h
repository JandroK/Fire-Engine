#pragma once
#include "Module.h"
#include "glmath.h"


class Camera3D : public Module
{
public:
	Camera3D(Application* app, bool start_enabled = true);
	~Camera3D() {};

	bool Start() override;
	update_status Update(float dt) override;
	void CheckInputs();
	bool CleanUp() override;

	void Look(const vec3 &Position, const vec3 &Reference, bool RotateAroundReference = false);
	void Look();
	void LookAt(const vec3 &Spot);
	void Move(const vec3 &Movement);
	float* GetViewMatrix();
	void OrbitRotation();

	void FrontView();

private:

	void CalculateViewMatrix();

	bool SaveConfig(JsonParser& node)const override;

	bool LoadConfig(JsonParser& node) override;

public:
	
	vec3 X, Y, Z, Position, Reference;

private:

	mat4x4 ViewMatrix, ViewMatrixInverse;
};