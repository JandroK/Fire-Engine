#include "Physics3D.h"
#include "Globals.h"
#include "Bullet/include/btBulletDynamicsCommon.h"

#ifdef _DEBUG
#pragma comment (lib, "BulletDynamics_debug.lib")
#pragma comment (lib, "BulletCollision_debug.lib")
#pragma comment (lib, "LinearMath_debug.lib")
#else
#pragma comment (lib, "BulletDynamics.lib")
#pragma comment (lib, "BulletCollision.lib")
#pragma comment (lib, "LinearMath.lib")
#endif

Physics3D::Physics3D(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	// Collision configuration contains default setup for memory, collision setup. Advanced
	// users can create their own configuration.
	collisionConfiguration = new btDefaultCollisionConfiguration();

	// Use the default collision dispatcher
	dispatcher = new btCollisionDispatcher(collisionConfiguration);

	// You can also try out	btAxis3Sweep or btBroadphaseInterface
	broadPhase = new btDbvtBroadphase();

	// The default constraint solver
	solver = new btSequentialImpulseConstraintSolver;

	debugDraw = new DebugDrawer();
}

Physics3D::~Physics3D()
{
	// Delete dynamics world
	RELEASE(debugDraw);
	RELEASE(solver);
	RELEASE(broadPhase);
	RELEASE(dispatcher);
	RELEASE(collisionConfiguration);
}

bool Physics3D::Init()
{
	return true;
}

bool Physics3D::Start()
{
	world = new btDiscreteDynamicsWorld(dispatcher, broadPhase, solver, collisionConfiguration);
	world->setGravity(btVector3(0, -10, 0));

	return true;
}

update_status Physics3D::PreUpdate(float dt)
{
	return UPDATE_CONTINUE;
}

update_status Physics3D::Update(float dt)
{
	return UPDATE_CONTINUE;
}

update_status Physics3D::PostUpdate(float dt)
{
	return UPDATE_CONTINUE;
}

bool Physics3D::CleanUp()
{
	// Remove the rigidbodies from the dynamics world and delete them
	for (int i = world->getNumCollisionObjects() - 1; i >= 0; i--)
	{
		btCollisionObject* obj = world->getCollisionObjectArray()[i];
		btRigidBody* body = btRigidBody::upcast(obj);
		if (body && body->getMotionState())
		{
			delete body->getMotionState();
		}
		world->removeCollisionObject(obj);
		RELEASE(obj);
	}

	for (std::list<btTypedConstraint*>::iterator it = constraints.begin(); it != constraints.end(); ++it)
		RELEASE(*it);
	constraints.clear();

	for (std::list<btDefaultMotionState*>::iterator it = motions.begin(); it != motions.end(); ++it)
		RELEASE(*it);
	motions.clear();

	for (std::list<btCollisionShape*>::iterator it = shapes.begin(); it != shapes.end(); ++it)
		RELEASE(*it);
	shapes.clear();

	for (std::list<PhysBody3D*>::iterator it = bodies.begin(); it != bodies.end(); ++it)
		RELEASE(*it);
	bodies.clear();

	for (std::list<PhysVehicle3D*>::iterator it = vehicles.begin(); it != vehicles.end(); ++it)
		RELEASE(*it);
	vehicles.clear();

	RELEASE(vehicleRaycaster);
	RELEASE(world);

	return true;
}

// =============================================
void DebugDrawer::drawLine(const btVector3& from, const btVector3& to, const btVector3& color)
{
	line.origin.Set(from.getX(), from.getY(), from.getZ());
	line.destination.Set(to.getX(), to.getY(), to.getZ());
	line.color.Set(color.getX(), color.getY(), color.getZ());
	line.Render();
}

void DebugDrawer::drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color)
{
	point.transform.translate(PointOnB.getX(), PointOnB.getY(), PointOnB.getZ());
	point.color.Set(color.getX(), color.getY(), color.getZ());
	point.Render();
}

void DebugDrawer::reportErrorWarning(const char* warningString)
{
	LOG(LogType::L_WARNING, "Bullet warning: %s", warningString);
}

void DebugDrawer::draw3dText(const btVector3& location, const char* textString)
{
	LOG(LogType::L_NORMAL, "Bullet draw text: %s", textString);
}

void DebugDrawer::setDebugMode(int debugMode)
{
	mode = (DebugDrawModes)debugMode;
}

int	 DebugDrawer::getDebugMode() const
{
	return mode;
}