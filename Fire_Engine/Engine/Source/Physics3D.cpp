#include "Physics3D.h"
#include "Globals.h"
#include "Application.h"
#include "Input.h"
#include "Camera3D.h"

#include "PhysBody3D.h"
#include "PhysVehicle3D.h"
#include "C_RigidBody.h"
#include "Bullet/include/btBulletDynamicsCommon.h"

#include "Geometry/GeometryAll.h"

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
	collisionConfig = new btDefaultCollisionConfiguration();

	// Use the default collision dispatcher
	dispatcher = new btCollisionDispatcher(collisionConfig);

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
	RELEASE(collisionConfig);
}

bool Physics3D::Init()
{
	LOG(LogType::L_NORMAL, "Creating 3D Physics simulation");
	return true;
}

bool Physics3D::Start()
{
	LOG(LogType::L_NORMAL, "Creating Physics environment");
	world = new btDiscreteDynamicsWorld(dispatcher, broadPhase, solver, collisionConfig);
	world->setDebugDrawer(debugDraw);
	world->setGravity(GRAVITY);
	vehicleRaycaster = new btDefaultVehicleRaycaster(world);

	return true;
}

update_status Physics3D::PreUpdate(float dt)
{
	world->stepSimulation(dt, 15);
	return UPDATE_CONTINUE;
}

update_status Physics3D::Update(float dt)
{
	if (app->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN)
		debug = !debug;

	if (debug == true)
	{
		world->debugDrawWorld();
	}

	if(App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)
	{
		PSphere s(1);
		s.SetPos(app->camera->GetPosition().x, app->camera->GetPosition().y, app->camera->GetPosition().z);
		float force = 30.0f;
		//CollisionShape(s, RB)->applyCentralImpulse()
		//AddBody(s)->Push(-(app->camera->GetFront().x * force), -(app->camera->GetFront().y * force), -(app->camera->GetFront().z * force));
	}

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

btRigidBody* Physics3D::CollisionShape(const PCube& cube, C_RigidBody* component)
{
	btCollisionShape* colShape = new btBoxShape(btVector3(cube.size.x * 0.5f, cube.size.y * 0.5f, cube.size.z * 0.5f));
	btTransform startTransform;
	startTransform.setFromOpenGLMatrix(&cube.transform);
	return AddBody(colShape, startTransform, (component->useGravity && !component->isKinematic) ? component->GetMass() : 0.0f);
}

btRigidBody* Physics3D::CollisionShape(const PSphere& sphere, C_RigidBody* component)
{
	btCollisionShape* colShape = new btSphereShape(sphere.radius);
	btTransform startTransform;
	startTransform.setFromOpenGLMatrix(&sphere.transform);
	return AddBody(colShape, startTransform, (component->useGravity && !component->isKinematic) ? component->GetMass() : 0.0f);
}

btRigidBody* Physics3D::CollisionShape(const PCapsule& capsule, C_RigidBody* component)
{
	btCollisionShape* colShape = new btCapsuleShape(capsule.radius, capsule.height);
	btTransform startTransform;
	startTransform.setFromOpenGLMatrix(&capsule.transform);
	return AddBody(colShape, startTransform, (component->useGravity && !component->isKinematic)?component->GetMass() : 0.0f);
}

btRigidBody* Physics3D::CollisionShape(const PCylinder& cylinder, C_RigidBody* component)
{
	btCollisionShape* colShape = new btCylinderShape(btVector3(cylinder.radius, cylinder.height * 0.5f, 0.0f));
	btTransform startTransform;
	startTransform.setFromOpenGLMatrix(&cylinder.transform);
	return AddBody(colShape, startTransform, (component->useGravity && !component->isKinematic) ? component->GetMass() : 0.0f);
}

btRigidBody* Physics3D::CollisionShape(const PPyramid& cone, C_RigidBody* component)
{
	btCollisionShape* colShape = new btConeShape(cone.radius, cone.height);
	btTransform startTransform;
	startTransform.setFromOpenGLMatrix(&cone.transform);
	return AddBody(colShape, startTransform, (component->useGravity && !component->isKinematic) ? component->GetMass() : 0.0f);
}

btRigidBody* Physics3D::CollisionShape(const PPlane& plane, C_RigidBody* component)
{
	btCollisionShape* colShape = new btStaticPlaneShape(btVector3(plane.normal.x, plane.normal.y, plane.normal.z), plane.constant);
	btTransform startTransform;
	startTransform.setFromOpenGLMatrix(&plane.transform);
	return AddBody(colShape, startTransform, (component->useGravity && !component->isKinematic) ? component->GetMass() : 0.0f);
}

btRigidBody* Physics3D::AddBody(btCollisionShape* colShape, btTransform startTransform, float mass)
{
	shapes.push_back(colShape);

	btVector3 localInertia(0, 0, 0);
	if (mass != 0.f)
		colShape->calculateLocalInertia(mass, localInertia);

	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
	motions.push_back(myMotionState);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShape, localInertia);

	btRigidBody* body = new btRigidBody(rbInfo);
	PhysBody3D* pbody = new PhysBody3D(body);

	body->setUserPointer(pbody);
	world->addRigidBody(body);
	bodies.push_back(pbody);

	return body;
}

void Physics3D::DeleteBody(btRigidBody* body)
{
	if (body != nullptr)
		world->removeRigidBody(body);
}

// =============================================
void Physics3D::AddConstraintP2P(PhysBody3D& bodyA, PhysBody3D& bodyB, const vec3& anchorA, const vec3& anchorB)
{
	btTypedConstraint* p2p = new btPoint2PointConstraint(
		*(bodyA.body),
		*(bodyB.body),
		btVector3(anchorA.x, anchorA.y, anchorA.z),
		btVector3(anchorB.x, anchorB.y, anchorB.z));
	world->addConstraint(p2p);
	constraints.push_back(p2p);
	p2p->setDbgDrawSize(2.0f);
}

void Physics3D::AddConstraintHinge(PhysBody3D& bodyA, PhysBody3D& bodyB, const vec3& anchorA, const vec3& anchorB, const vec3& axisA, const vec3& axisB, bool disable_collision)
{
	btHingeConstraint* hinge = new btHingeConstraint(
		*(bodyA.body),
		*(bodyB.body),
		btVector3(anchorA.x, anchorA.y, anchorA.z),
		btVector3(anchorB.x, anchorB.y, anchorB.z),
		btVector3(axisA.x, axisA.y, axisA.z),
		btVector3(axisB.x, axisB.y, axisB.z));

	world->addConstraint(hinge, disable_collision);
	constraints.push_back(hinge);
	hinge->setDbgDrawSize(2.0f);
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