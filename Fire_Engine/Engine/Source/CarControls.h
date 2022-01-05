#pragma once
#include "LinearMath/btVector3.h"
struct PhysVehicle3D;

#define MAX_ACCELERATION 400.0f
#define TURN_DEGREES 17.0f * DEGTORAD
#define BRAKE_POWER 50.0f

class CarControls
{
public:
	CarControls();
	~CarControls();

	update_status Update();
	void PlayerControls();
	void AssistDirection(float hardness);

	PhysVehicle3D* vehicle = nullptr;
private:
	float acceleration = 0.0f;
	float vel = 0.0f;
	float turn = 0.0f;
	float brake = 0.0f;
	float assistDirection = 0.0f;
	float calculate = 0.0f;

	btVector3 forwardVector;
	btVector3 perpendicularVector;
};

