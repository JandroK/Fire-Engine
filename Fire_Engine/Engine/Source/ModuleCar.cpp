#include "Application.h"
#include "ModuleCar.h"
#include "Input.h"

#include "PhysVehicle3D.h"
#include "PhysBody3D.h"
#include "Physics3D.h"
#include "Bullet/include/btBulletDynamicsCommon.h"

Car::Car(Application* app, bool start_enabled) : Module(app, start_enabled)
{
}

Car::~Car()
{
}

bool Car::Start()
{	
	// Vehicle chassis ---------------------------------------
	VehicleInfo car;
	car.chassis_size.Set(1, 0.5f, 2);
	car.chassis_offset.Set(0, 0.125f, 0.05);

	// Car properties ----------------------------------------
	car.mass = 130.0f;
	car.suspensionStiffness = 26.10f;
	car.suspensionCompression = 1.42f;
	car.suspensionDamping = 2.35f;
	car.maxSuspensionTravelCm = 510;
	car.frictionSlip = 100.5;
	car.maxSuspensionForce = 1000.0f;

	// Wheel properties ---------------------------------------
	float connection_height = car.chassis_size.y - car.chassis_offset.z + 0.75;
	float wheel_radius = 0.6f;
	float wheel_width = 0.75f;
	float suspensionRestLength = 1.25f;
	float half_width = car.chassis_size.x + 0.28f;
	float half_length = car.chassis_size.z * 0.6f;
	float3 direction(0, -1, 0);
	float3 axis(-1, 0, 0);

	car.num_wheels = 4;
	car.wheels = new Wheel[4];

	// FRONT-LEFT ------------------------
	car.wheels[0].connection.Set(half_width - 0.4f * wheel_width, connection_height, half_length - wheel_radius + 0.2 + car.chassis_offset.z);
	car.wheels[0].direction = direction;
	car.wheels[0].axis = axis;
	car.wheels[0].suspensionRestLength = suspensionRestLength;
	car.wheels[0].radius = wheel_radius * 0.95;
	car.wheels[0].width = wheel_width * 0.9;
	car.wheels[0].front = true;
	car.wheels[0].drive = true;
	car.wheels[0].brake = false;
	car.wheels[0].steering = true;

	// FRONT-RIGHT ------------------------
	car.wheels[1].connection.Set(-half_width + 0.4f * wheel_width, connection_height, half_length - wheel_radius + 0.2 + car.chassis_offset.z);
	car.wheels[1].direction = direction;
	car.wheels[1].axis = axis;
	car.wheels[1].suspensionRestLength = suspensionRestLength;
	car.wheels[1].radius = wheel_radius * 0.95;
	car.wheels[1].width = wheel_width * 0.9;
	car.wheels[1].front = true;
	car.wheels[1].drive = true;
	car.wheels[1].brake = false;
	car.wheels[1].steering = true;

	// REAR-LEFT ------------------------
	car.wheels[2].connection.Set(half_width - 0.3f * wheel_width, connection_height, -half_length + wheel_radius - 0.3 + car.chassis_offset.z);
	car.wheels[2].direction = direction;
	car.wheels[2].axis = axis;
	car.wheels[2].suspensionRestLength = suspensionRestLength;
	car.wheels[2].radius = wheel_radius;
	car.wheels[2].width = wheel_width;
	car.wheels[2].front = false;
	car.wheels[2].drive = false;
	car.wheels[2].brake = true;
	car.wheels[2].steering = false;

	// REAR-RIGHT ------------------------
	car.wheels[3].connection.Set(-half_width + 0.3f * wheel_width, connection_height, -half_length + wheel_radius - 0.3 + car.chassis_offset.z);
	car.wheels[3].direction = direction;
	car.wheels[3].axis = axis;
	car.wheels[3].suspensionRestLength = suspensionRestLength;
	car.wheels[3].radius = wheel_radius;
	car.wheels[3].width = wheel_width;
	car.wheels[3].front = false;
	car.wheels[3].drive = false;
	car.wheels[3].brake = true;
	car.wheels[3].steering = false;

	vehicle = app->physics->AddVehicle(car);

	vehicle->body->setFriction(1);
	vehicle->collision_listeners.push_back(this);
	vehicle->body->setUserPointer(vehicle);
	vehicle->body->setWorldTransform(btTransform::getIdentity());
	vehicle->SetPos(0, 5, 0);

	return true;
}

update_status Car::Update(float dt)
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

update_status Car::PostUpdate(float dt)
{
	vehicle->Render();
	return update_status::UPDATE_CONTINUE;
}

bool Car::CleanUp()
{
	return true;
}

void Car::PlayerControls()
{
	if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT &&
		(vehicle->state != State::IN_AIR || vehicle->state == State::TURBO) &&
		App->input->GetKey(SDL_SCANCODE_S) != KEY_REPEAT)
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

	if (app->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
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
	}
}

void Car::AssistDirection(float hardness)
{
	// FUYM which reduces the amount of spin the wheel can exert relative to the amount of speed 
	float turnDegrees = (TURN_DEGREES / DEGTORAD);
	calculate = (vehicle->GetKmh() / 16) * (hardness / 100.0f);
	if (calculate <= turnDegrees - 5)
		assistDirection = calculate * DEGTORAD;
	else assistDirection = (turnDegrees - 5) * DEGTORAD;
}
