#pragma once
#include "Module.h"

#include "Math/float3.h"

#include "ComponentCamera.h"

class Camera3D : public Module
{
public:
	Camera3D(Application* app, bool start_enabled = true);
	~Camera3D() {};

	void ReStartCamera();
	bool Start() override;

	update_status Update(float dt) override;
	void OnGUI() override;

	void CheckInputs();
	bool CleanUp() override;

	void LookAt(const float3&Spot);
	void Move(const float3&Movement);

	void OrbitRotation();
	void CalculateViewMatrix();

	bool GetUpdateAspectRatio() { return isUpdateAspectRatio; };
	void SetUpdateAspectRatio(bool ret) { isUpdateAspectRatio = ret; };

private:
	void Focus();
	void FrontView();

	bool SaveConfig(JsonParser& node)const override;
	bool LoadConfig(JsonParser& node) override;

public:
	
	float3 right, up, front, position, reference;

	bool projectionIsDirty = false;
	bool isUpdateAspectRatio = false;

	ComponentCamera cameraScene;

	float cameraSpeed = 5.0f;
	float cameraSensitivity = 0.15f;
};