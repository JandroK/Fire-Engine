#pragma once
#include "Module.h"

class btDefaultCollisionConfiguration;
class btCollisionDispatcher;
class btBroadphaseInterface;
class btSequentialImpulseConstraintSolver;
class btDiscreteDynamicsWorld;

class Physics3D : public Module
{
public:
	Physics3D(Application* app, bool start_enabled = true);
	~Physics3D() {};

	bool Init() override;
	bool Start() override;
	bool CleanUp() override;

private:
	btDefaultCollisionConfiguration* collisionConfiguration = nullptr;
	btCollisionDispatcher* dispatcher = nullptr;
	btBroadphaseInterface* broadPhase = nullptr;
	btSequentialImpulseConstraintSolver* solver = nullptr;
	btDiscreteDynamicsWorld * world = nullptr;

};

