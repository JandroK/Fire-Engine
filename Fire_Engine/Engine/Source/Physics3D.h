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
struct VehicleInfo;

class btRigidBody;
class C_RigidBody;

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

	btRigidBody* CollisionShape(const PCube& cube, C_RigidBody* component);
	btRigidBody* CollisionShape(const PSphere& sphere, C_RigidBody* component);
	btRigidBody* CollisionShape(const PCapsule& capsule, C_RigidBody* component);
	btRigidBody* CollisionShape(const PCylinder& cylinder, C_RigidBody* component);
	btRigidBody* CollisionShape(const PPyramid& cone, C_RigidBody* component);
	btRigidBody* CollisionShape(const PPlane& plane, C_RigidBody* component);

	btRigidBody* AddBody(btCollisionShape* colShape, btTransform startTransform, C_RigidBody* component);
	PhysVehicle3D* AddVehicle(const VehicleInfo& info);
	void DeleteBody(C_RigidBody* body, std::string name);
	void DesactivateCollision(btRigidBody* body);
	void ActivateCollision(btRigidBody* body);

	void AddConstraintP2P(btRigidBody& bodyA, btRigidBody& bodyB, const float3& anchorA, const float3& anchorB);
	void AddConstraintHinge(btRigidBody& bodyA, btRigidBody& bodyB, const float3& anchorA, const float3& anchorB, const float3& axisS, const float3& axisB, bool disable_collision = false);

	std::vector<C_RigidBody*> GetBodies() { return bodies; };
	std::vector<std::string> GetBodiesNames() { return bodiesNames; };

	std::vector<std::string> bodiesNames;
private:
	btDefaultCollisionConfiguration* collisionConfig = nullptr;
	btCollisionDispatcher* dispatcher = nullptr;
	btBroadphaseInterface* broadPhase = nullptr;
	btSequentialImpulseConstraintSolver* solver = nullptr;
	btDiscreteDynamicsWorld * world = nullptr;

	btDefaultVehicleRaycaster* vehicleRaycaster = nullptr;
	DebugDrawer* debugDraw = nullptr;

	std::list<btCollisionShape*> shapes;
	std::vector<C_RigidBody*> bodies;
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

	DebugDrawModes mode = DBG_DrawWireframe;
	PLine line;
	Primitive point;
};

