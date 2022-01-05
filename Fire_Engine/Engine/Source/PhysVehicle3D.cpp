#include "PhysVehicle3D.h"
#include "Application.h"
#include "Bullet/include/btBulletDynamicsCommon.h"
#include "Scene.h"

VehicleInfo::VehicleInfo(float3 size, float mass)
{
	// Vehicle chassis ---------------------------------------
	float3 chassis_offset = float3(0, 0.125f, 0.05);
	chassis_size = size;

	// Car properties ----------------------------------------
	this->mass = mass;
	suspensionStiffness = 26.10f;
	suspensionCompression = 1.42f;
	suspensionDamping = 2.35f;
	maxSuspensionTravelCm = 510;
	frictionSlip = 100.5;
	maxSuspensionForce = 1000.0f;

	// Wheel properties ---------------------------------------
	float wheel_radius = 0.6f;
	float wheel_width = 0.75f;
	float connection_height = -chassis_size.y * 0.5f + wheel_radius * 1.25f;
	float suspensionRestLength = 1.25f;
	float half_width = chassis_size.x * 0.5f;
	float half_length = chassis_size.z * 0.25f;
	float3 direction(0, -1, 0);
	float3 axis(-1, 0, 0);

	num_wheels = 4;
	wheels = new Wheel[4];

	// FRONT-LEFT ------------------------
	wheels[0].connection.Set(-half_width - 0.5f * wheel_width, connection_height, half_length);
	wheels[0].direction = direction;
	wheels[0].axis = axis;
	wheels[0].suspensionRestLength = suspensionRestLength;
	wheels[0].radius = wheel_radius * 0.95;
	wheels[0].width = wheel_width * 0.9;
	wheels[0].front = true;
	wheels[0].drive = true;
	wheels[0].brake = false;
	wheels[0].steering = true;

	// FRONT-RIGHT ------------------------
	wheels[1].connection.Set(half_width + 0.5f * wheel_width , connection_height, half_length);
	wheels[1].direction = direction;
	wheels[1].axis = axis;
	wheels[1].suspensionRestLength = suspensionRestLength;
	wheels[1].radius = wheel_radius * 0.95;
	wheels[1].width = wheel_width * 0.9;
	wheels[1].front = true;
	wheels[1].drive = true;
	wheels[1].brake = false;
	wheels[1].steering = true;

	// REAR-LEFT ------------------------
	wheels[2].connection.Set(-half_width - 0.5f * wheel_width, connection_height, -half_length);
	wheels[2].direction = direction;
	wheels[2].axis = axis;
	wheels[2].suspensionRestLength = suspensionRestLength;
	wheels[2].radius = wheel_radius;
	wheels[2].width = wheel_width;
	wheels[2].front = false;
	wheels[2].drive = false;
	wheels[2].brake = true;
	wheels[2].steering = false;

	// REAR-RIGHT ------------------------
	wheels[3].connection.Set(half_width + 0.5f * wheel_width, connection_height, -half_length);
	wheels[3].direction = direction;
	wheels[3].axis = axis;
	wheels[3].suspensionRestLength = suspensionRestLength;
	wheels[3].radius = wheel_radius;
	wheels[3].width = wheel_width;
	wheels[3].front = false;
	wheels[3].drive = false;
	wheels[3].brake = true;
	wheels[3].steering = false;
}

// ----------------------------------------------------------------------------
VehicleInfo::~VehicleInfo()
{
	//if(wheels != nullptr)
		//RELEASE(wheels);
}

// ----------------------------------------------------------------------------
PhysVehicle3D::PhysVehicle3D(btRaycastVehicle* vehicle, const VehicleInfo& info) : vehicle(vehicle), info(info)
{
}

// ----------------------------------------------------------------------------
PhysVehicle3D::~PhysVehicle3D()
{
	RELEASE(vehicle);
}

// ----------------------------------------------------------------------------
void PhysVehicle3D::Render()
{
	for (int i = 0; i < vehicle->getNumWheels(); ++i)
	{
		wheel.radius = info.wheels[i].radius;
		wheel.height = info.wheels[i].width;

		vehicle->updateWheelTransform(i);
		vehicle->getWheelInfo(i).m_worldTransform.getOpenGLMatrix(&wheel.transform);

		wheel.color = info.wheels[i].color;
		if (state == TURBO)
			if ((i == 3 || i == 2)) // Rear wheels
				wheel.color = Red;

		wheel.Render();
	}
}

// ----------------------------------------------------------------------------
void PhysVehicle3D::ApplyEngineForce(float force)
{
	for (int i = 0; i < vehicle->getNumWheels(); ++i)
	{
		if (info.wheels[i].drive == true)
		{
			vehicle->applyEngineForce(force, i);
		}
	}
}

// ----------------------------------------------------------------------------
void PhysVehicle3D::Brake(float force)
{
	for (int i = 0; i < vehicle->getNumWheels(); ++i)
	{
		if (info.wheels[i].brake == true)
		{
			vehicle->setBrake(force, i);
		}
	}
}

// ----------------------------------------------------------------------------
void PhysVehicle3D::Turn(float degrees)
{
	for (int i = 0; i < vehicle->getNumWheels(); ++i)
	{
		if (info.wheels[i].steering == true)
		{
			vehicle->setSteeringValue(degrees, i);
		}
	}
}

// ----------------------------------------------------------------------------
float PhysVehicle3D::GetKmh() const
{
	return vehicle->getCurrentSpeedKmHour();
}
