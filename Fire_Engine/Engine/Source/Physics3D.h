#pragma once
#include "Module.h"
#include "Primitive.h"
#include <list>
#include "Bullet/include/LinearMath/btIDebugDraw.h"

#define GRAVITY btVector3(0.0f, -10.0f, 0.0f)

class btDefaultCollisionConfiguration;
class btCollisionDispatcher;
class btBroadphaseInterface;
class btSequentialImpulseConstraintSolver;
class btDiscreteDynamicsWorld;

class btDefaultVehicleRaycaster;
class DebugDrawer;

class btCollisionShape;
class btDefaultMotionState;
class btTypedConstraint;
struct PhysBody3D;
struct PhysVehicle3D;

class Physics3D : public Module
{
public:
	Physics3D(Application* app, bool start_enabled = true);
	~Physics3D();

	bool Init() override;
	bool Start() override;
	update_status PreUpdate(float dt) override;
	update_status Update(float dt) override;
	update_status PostUpdate(float dt) override;
	bool CleanUp() override;

private:
	btDefaultCollisionConfiguration* collisionConfiguration = nullptr;
	btCollisionDispatcher* dispatcher = nullptr;
	btBroadphaseInterface* broadPhase = nullptr;
	btSequentialImpulseConstraintSolver* solver = nullptr;
	btDiscreteDynamicsWorld * world = nullptr;

	btDefaultVehicleRaycaster* vehicleRaycaster = nullptr;
	DebugDrawer* debugDraw = nullptr;

	std::list<btCollisionShape*> shapes;
	std::list<PhysBody3D*> bodies;
	std::list<btDefaultMotionState*> motions;
	std::list<btTypedConstraint*> constraints;
	std::list<PhysVehicle3D*> vehicles;

	bool debug = true;
};

class DebugDrawer : public btIDebugDraw
{
public:
	DebugDrawer() : line(0,0,0)
	{}

	// This use camelCase because use virtual functions of btIDebugDraw
	void drawLine(const btVector3& from, const btVector3& to, const btVector3& color);
	void drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color);
	void reportErrorWarning(const char* warningString);
	void draw3dText(const btVector3& location, const char* textString);
	void setDebugMode(int debugMode);
	int	 getDebugMode() const;

	DebugDrawModes mode;
	PrimitiveLine line;
	Primitive point;
};

