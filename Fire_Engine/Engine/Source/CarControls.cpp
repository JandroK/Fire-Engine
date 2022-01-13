#include "Application.h"
#include "CarControls.h"
#include "Input.h"

#include "PhysVehicle3D.h"
#include "Physics3D.h"
#include "Bullet/include/btBulletDynamicsCommon.h"

#include "ImGui/imgui.h"

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
	AssistDirection(hardnessPS);
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
		vel = maxAcceleration * 2;
		vehicle->state = TURBO;
		// FUYM car tilt
		//vehicle->vehicle->getRigidBody()->applyCentralForce({ 0,-99,0 });
	}
	else
	{
		vel = maxAcceleration;
		if (vehicle->state != State::IN_AIR)vehicle->state = State::IDLE;
	}

	if (app->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
	{
		if (vehicle->state != State::TURBO && vehicle->state != State::IN_AIR)
			vehicle->state = State::WALK;
		//vehicle->vehicle->getRigidBody()->applyCentralForce({ 0,-70,0 });

		if (-vehicle->GetKmh() < -2.5)
		{
			brake = breakPower;
			LOG(LogType::L_NORMAL, "Accelerando");
			//vehicle->vehicle->getRigidBody()->applyCentralForce({ 0,-0.1,0 });
		}
		acceleration = vel;
	}

	if (app->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
	{
		if (vehicle->state != State::TURBO && vehicle->state != State::IN_AIR)
			vehicle->state = State::WALK;

		if (-vehicle->GetKmh() < -2.5)
		{
			brake = breakPower;
			LOG(LogType::L_NORMAL, "Frenando");
			//vehicle->vehicle->getRigidBody()->applyCentralForce({ 0,-0.1,0 });
		}
		acceleration = -vel;
	}

	if (app->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
	{
		if (turn < maxTurnDegrees * DEGTORAD)
			turn += (maxTurnDegrees * DEGTORAD)-assistDirection;
		brake = 10;

		if (vehicle->state == State::IN_AIR)
			vehicle->vehicle->getRigidBody()->applyTorque(forwardVector * 45);
		else
			vehicle->vehicle->getRigidBody()->applyTorque(forwardVector * 200);
	}

	if (app->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
	{
		if (turn > -maxTurnDegrees * DEGTORAD)
			turn -= (maxTurnDegrees * DEGTORAD)-assistDirection;
		brake = 10;

		if (vehicle->state == State::IN_AIR)
			vehicle->vehicle->getRigidBody()->applyTorque(forwardVector * -45);
		else
			vehicle->vehicle->getRigidBody()->applyTorque(forwardVector * -200);
	}
}

void CarControls::AssistDirection(float hardness)
{
	// FUYM which reduces the amount of spin the wheel can exert relative to the amount of speed 
	float turnDegrees = (maxTurnDegrees);
	calculate = (vehicle->GetKmh() / 16) * (hardness / 100.0f);
	if (calculate <= turnDegrees - 5)
		assistDirection = calculate * DEGTORAD;
	else assistDirection = (turnDegrees - 5) * DEGTORAD;
}

void CarControls::OnEditor()
{
	float size = ImGui::GetWindowSize().x - ImGui::CalcTextSize("Max Acceleration:").x - 40;
	
	ImGui::Text("Max Acceleration:"); ImGui::SameLine();
	ImGui::PushItemWidth(size);
	ImGui::DragFloat("##MaxAcceleration", &maxAcceleration, 1, 0, INFINITE);
	ImGui::PopItemWidth();

	ImGui::Text("Max Turn Degrees:"); ImGui::SameLine();
	ImGui::PushItemWidth(size);
	ImGui::DragFloat("##MaxTurnDegrees", &maxTurnDegrees, 1, 0, INFINITE);
	ImGui::PopItemWidth();

	ImGui::Text("Hardness PS:     "); ImGui::SameLine();
	ImGui::PushItemWidth(size);
	ImGui::DragFloat("##HardnessPS", &hardnessPS, 1, 0, INFINITE);
	ImGui::PopItemWidth();
}
