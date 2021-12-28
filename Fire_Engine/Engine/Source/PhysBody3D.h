#pragma once

#include <list>
#include "Math/float3.h"

class btRigidBody;
class btVector3;
class Module;

// =================================================
struct PhysBody3D
{
public:
	PhysBody3D(btRigidBody* body);
	~PhysBody3D();

	void Push(float x, float y, float z);

	void GetTransform(float* matrix) const;
	void SetTransform(const float* matrix) const;

	void SetPos(float x, float y, float z);
	void SetPos(const btVector3& vecPos);
	void SetPos(float3& vecPos);

	void SetAsSensor(bool is_sensor);
	bool GetIsSensor() { return isSensor; }

private:
	bool isSensor = false;
public:
	btRigidBody* body = nullptr;
	std::list<Module*> collision_listeners;
};

