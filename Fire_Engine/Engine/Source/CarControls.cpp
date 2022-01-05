#include "Application.h"
#include "CarControls.h"
#include "Input.h"

#include "PhysVehicle3D.h"
#include "Physics3D.h"
#include "Bullet/include/btBulletDynamicsCommon.h"

CarControls::CarControls()
{
}

CarControls::~CarControls()
{
}

update_status CarControls::Update()
{
	brake = 2.5f;
	turn = acceleration = 0.0f;
	AssistDirection(90.0f);
	forwardVector = vehicle->vehicle->getForwardVector().normalize();
	perpendicularVector = btVector3(-forwardVector.getZ(), forwardVector.getY(), forwardVector.getX());

	PlayerControls();

	vehicle->ApplyEngineForce(acceleration);
	vehicle->Turn(turn);
	vehicle->Brake(brake);

	return update_status::UPDATE_CONTINUE;
}

void CarControls::PlayerControls()
{
	if (app->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT &&
		(vehicle->state != State::IN_AIR || vehicle->state == State::TURBO) &&
		app->input->GetKey(SDL_SCANCODE_S) != KEY_REPEAT)
	{
		vel = MAX_ACCELERATION * 2;
		vehicle->state = TURBO;
		// FUYM car tilt
		vehicle->vehicle->getRigidBody()->applyCentralForce({ 0,-99,0 });
	}
	else
	{
		vel = MAX_ACCELERATION;
		if (vehicle->state != State::IN_AIR)vehicle->state = State::IDLE;
	}

	if (app->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
	{
		if (vehicle->state != State::TURBO && vehicle->state != State::IN_AIR)
			vehicle->state = State::WALK;
		//vehicle->vehicle->getRigidBody()->applyCentralForce({ 0,-70,0 });

		if (vehicle->vehicle->getCurrentSpeedKmHour() <= -2.25)
		{
			//brake = BRAKE_POWER / 1.5f;
			// FUYM to simulate restitution	
			//vehicle->vehicle->getRigidBody()->applyCentralForce({ 0,-200,0 });
		}
		else
			acceleration = vel;
	}

	if (app->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
	{
		if (vehicle->state != State::TURBO && vehicle->state != State::IN_AIR)
			vehicle->state = State::WALK;

		if (vehicle->vehicle->getCurrentSpeedKmHour() > +2.25)
		{
			brake = BRAKE_POWER / 1.5f;	
			vehicle->vehicle->getRigidBody()->applyCentralForce({ 0,-200,0 });
		}
		else
			acceleration = vel * -1;
	}

	/*if (app->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
	{
		if (turn < TURN_DEGREES)
			turn += (TURN_DEGREES)-assistDirection;
		brake = 10;

		if (vehicle->state == State::IN_AIR)
			vehicle->body->applyTorque(forwardVector * 45);
		else
			vehicle->body->applyTorque(forwardVector * 200);
	}

	if (app->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
	{
		if (turn > -TURN_DEGREES)
			turn -= (TURN_DEGREES)-assistDirection;
		brake = 10;

		if (vehicle->state == State::IN_AIR)
			vehicle->body->applyTorque(forwardVector * -45);
		else
			vehicle->body->applyTorque(forwardVector * -200);
	}*/
}

void CarControls::AssistDirection(float hardness)
{
	// FUYM which reduces the amount of spin the wheel can exert relative to the amount of speed 
	float turnDegrees = (TURN_DEGREES / DEGTORAD);
	calculate = (vehicle->GetKmh() / 16) * (hardness / 100.0f);
	if (calculate <= turnDegrees - 5)
		assistDirection = calculate * DEGTORAD;
	else assistDirection = (turnDegrees - 5) * DEGTORAD;
}
