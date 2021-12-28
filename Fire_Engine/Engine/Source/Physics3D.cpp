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
}

bool Physics3D::Init()
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

	return true;
}

bool Physics3D::Start()
{
	world = new btDiscreteDynamicsWorld(dispatcher, broadPhase, solver, collisionConfiguration);
	world->setGravity(btVector3(0, -10, 0));

	return true;
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

	// Delete dynamics world
	RELEASE(world);
	RELEASE(solver);
	RELEASE(broadPhase);
	RELEASE(dispatcher);
	RELEASE(collisionConfiguration);

	return true;
}
