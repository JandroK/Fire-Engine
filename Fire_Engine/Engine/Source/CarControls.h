#pragma once
#include "Math/float3.h"
struct PhysVehicle3D;

class CarControls
{
public:
	CarControls();
	~CarControls();

	update_status Update();
	void PlayerControls();
	void AssistDirection(float hardness);

	void OnEditor();

	PhysVehicle3D* vehicle = nullptr;

private:
	float acceleration = 0.0f;
	float vel = 0.0f;
	float turn = 0.0f;
	float brake = 0.0f;
	float assistDirection = 0.0f;
	float calculate = 0.0f;

	float3 forwardVector;
	float3 perpendicularVector;

	float maxAcceleration = 400.0f;
	float maxTurnDegrees = 17.0f;
	float breakPower = 30.0f;
	float hardnessPS = 90.0f;
};

