#pragma once
#include "Module.h"
#include "LinearMath/btVector3.h"
struct PhysVehicle3D;

#define MAX_ACCELERATION 400.0f
#define TURN_DEGREES 17.0f * DEGTORAD
#define BRAKE_POWER 50.0f

class Car : public Module
{
public:
	Car(Application* app, bool start_enabled = true);
	virtual ~Car();

	bool Start() override;
	update_status Update(float dt) override;
	update_status PostUpdate(float dt) override;
	bool CleanUp() override;

	void PlayerControls();
	void AssistDirection(float hardness);

private:
	PhysVehicle3D* vehicle = nullptr;
	float acceleration = 0.0f;
	float vel = 0.0f;
	float turn = 0.0f;
	float brake = 0.0f;
	float assistDirection = 0.0f;
	float calculate = 0.0f;

	btVector3 forwardVector;
	btVector3 perpendicularVector;
};

