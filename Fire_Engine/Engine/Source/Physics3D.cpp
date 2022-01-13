#include "Physics3D.h"
#include "Globals.h"
#include "Application.h"
#include "Input.h"
#include "Camera3D.h"
#include "Scene.h"

#include "PhysVehicle3D.h"
#include "C_RigidBody.h"
#include "Bullet/include/btBulletDynamicsCommon.h"

#include "DTEngine.h"

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
	if (DTEngine::state == DTGState::PLAY)
	{
		int numManifolds = world->getDispatcher()->getNumManifolds();
		for (int i = 0; i < numManifolds; i++)
		{
			btPersistentManifold* contactManifold = world->getDispatcher()->getManifoldByIndexInternal(i);
			btCollisionObject* obA = (btCollisionObject*)(contactManifold->getBody0());
			btCollisionObject* obB = (btCollisionObject*)(contactManifold->getBody1());

			int numContacts = contactManifold->getNumContacts();

			if (numContacts > 0 && (obA == app->camera->rigidBody->GetBody() || obB == app->camera->rigidBody->GetBody()))
			{
				app->camera->collision = true;
			}
		}
	}
	
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

	if(App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN && DTEngine::state == DTGState::PLAY)
	{
		PSphere spherePrim = PSphere(1);
		spherePrim.SetPos(app->camera->GetPosition().x, app->camera->GetPosition().y, app->camera->GetPosition().z);
		spherePrim.InnerMesh();
		spherePrim.mesh->LoadToMemory();
		spherePrim.mesh->GenerateBounds();

		float force = 30.0f;
		GameObject* s = app->scene->CreatePrimitive("Sphere", spherePrim.mesh);
		s->transform->SetWorldPosition(app->camera->GetPosition());
		s->transform->UpdateTransform();
		C_RigidBody* rigidBody;
		rigidBody = static_cast<C_RigidBody*>(s->AddComponent(ComponentType::RIGIDBODY));
		rigidBody->SetCollisionType(CollisionType::SPHERE);
		rigidBody->GetBody()->setIgnoreCollisionCheck(app->camera->rigidBody->GetBody(), true);
		rigidBody->GetBody()->applyCentralImpulse(app->camera->GetFront().Normalized() * force);
	}

	return UPDATE_CONTINUE;
}

update_status Physics3D::PostUpdate(float dt)
{
	for (int i = 0; i < vehicles.size(); i++)
	{
		vehicles.at(i)->Render();
	}

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

	/*for (std::vector<C_RigidBody*>::iterator it = bodies.begin(); it != bodies.end(); ++it)
		RELEASE(*it);*/
	bodies.clear();
	bodiesNames.clear();

	for (std::vector<PhysVehicle3D*>::iterator it = vehicles.begin(); it != vehicles.end(); ++it)
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
	return AddBody(colShape, startTransform, component);
}

btRigidBody* Physics3D::CollisionShape(const PSphere& sphere, C_RigidBody* component)
{
	btCollisionShape* colShape = new btSphereShape(sphere.radius);
	btTransform startTransform;
	startTransform.setFromOpenGLMatrix(&sphere.transform);
	return AddBody(colShape, startTransform, component);
}

btRigidBody* Physics3D::CollisionShape(const PCapsule& capsule, C_RigidBody* component)
{
	btCollisionShape* colShape = new btCapsuleShape(capsule.radius, capsule.height);
	btTransform startTransform;
	startTransform.setFromOpenGLMatrix(&capsule.transform);
	return AddBody(colShape, startTransform, component);
}

btRigidBody* Physics3D::CollisionShape(const PCylinder& cylinder, C_RigidBody* component)
{
	btCollisionShape* colShape = new btCylinderShape(btVector3(cylinder.radius, cylinder.height * 0.5f, 0.0f));
	btTransform startTransform;
	startTransform.setFromOpenGLMatrix(&cylinder.transform);
	return AddBody(colShape, startTransform, component);
}

btRigidBody* Physics3D::CollisionShape(const PPyramid& cone, C_RigidBody* component)
{
	btCollisionShape* colShape = new btConeShape(cone.radius, cone.height);
	btTransform startTransform;
	startTransform.setFromOpenGLMatrix(&cone.transform);
	return AddBody(colShape, startTransform, component);
}

btRigidBody* Physics3D::CollisionShape(const PPlane& plane, C_RigidBody* component)
{
	btCollisionShape* colShape = new btStaticPlaneShape(btVector3(plane.normal.x, plane.normal.y, plane.normal.z), plane.constant);
	btTransform startTransform;
	startTransform.setFromOpenGLMatrix(&plane.transform);
	return AddBody(colShape, startTransform, component);
}

btRigidBody* Physics3D::AddBody(btCollisionShape* colShape, btTransform startTransform, C_RigidBody* component)
{
	float mass = (component->useGravity && !component->isKinematic) ? component->GetMass() : 0.0f;
	shapes.push_back(colShape);

	btVector3 localInertia(0, 0, 0);
	if (mass != 0.f)
		colShape->calculateLocalInertia(mass, localInertia);

	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
	motions.push_back(myMotionState);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShape, localInertia);

	btRigidBody* body = new btRigidBody(rbInfo);

	if (component->isKinematic)
	{
		body->setCollisionFlags(body->getFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
		body->setActivationState(DISABLE_DEACTIVATION);
	}		
	if (DTEngine::state != DTGState::PLAY)
		body->setActivationState(ISLAND_SLEEPING);

	world->addRigidBody(body);
	bodies.push_back(component);
	if(component->GetCollisionType() != CollisionType::CAMERA)
		bodiesNames.push_back(component->GetOwner()->name);
	else bodiesNames.push_back("Camera");

	return body;
}

PhysVehicle3D* Physics3D::AddVehicle(const VehicleInfo& info, btRigidBody* body)
{
	body->setContactProcessingThreshold(BT_LARGE_FLOAT);
	if (DTEngine::state == DTGState::PLAY)
		body->setActivationState(DISABLE_DEACTIVATION);

	btRaycastVehicle::btVehicleTuning tuning;
	tuning.m_frictionSlip = info.frictionSlip;
	tuning.m_maxSuspensionForce = info.maxSuspensionForce;
	tuning.m_maxSuspensionTravelCm = info.maxSuspensionTravelCm;
	tuning.m_suspensionCompression = info.suspensionCompression;
	tuning.m_suspensionDamping = info.suspensionDamping;
	tuning.m_suspensionStiffness = info.suspensionStiffness;

	btRaycastVehicle* vehicle = new btRaycastVehicle(tuning, body, vehicleRaycaster);

	for (int i = 0; i < info.num_wheels; ++i)
	{
		btVector3 conn(info.wheels[i].connection.x, info.wheels[i].connection.y, info.wheels[i].connection.z);
		btVector3 dir(info.wheels[i].direction.x, info.wheels[i].direction.y, info.wheels[i].direction.z);
		btVector3 axis(info.wheels[i].axis.x, info.wheels[i].axis.y, info.wheels[i].axis.z);

		vehicle->addWheel(conn, dir, axis, info.wheels[i].suspensionRestLength, info.wheels[i].radius, tuning, info.wheels[i].front);
	}

	PhysVehicle3D* pvehicle = new PhysVehicle3D(vehicle, info);
	world->addVehicle(vehicle);
	vehicles.push_back(pvehicle);

	return pvehicle;
}

void Physics3D::DeleteBody(C_RigidBody* body, std::string name)
{
	if (body != nullptr)
	{
		world->removeRigidBody(body->GetBody());
		if (body->GetVehicle() != nullptr)
		{
			world->removeVehicle(body->GetVehicle()->vehicle);
			for (std::vector<PhysVehicle3D*>::iterator i = vehicles.begin(); i != vehicles.end(); ++i)
			{
				if (*i._Ptr == body->GetVehicle())
				{
					vehicles.erase(i);
					break;
				}
			}
		}
		for (std::vector<C_RigidBody*>::iterator i = bodies.begin(); i != bodies.end(); ++i)
		{
			if (*i._Ptr == body)
			{
				bodies.erase(i);
				break;
			}
		}
		for (std::vector<std::string>::iterator i = bodiesNames.begin(); i != bodiesNames.end(); ++i)
		{
			if (*i._Ptr == name)
			{
				bodiesNames.erase(i);
				break;
			}
		}
	}
}

void Physics3D::DesactivateCollision(btRigidBody* body)
{
	world->removeCollisionObject(body);
}
void Physics3D::ActivateCollision(btRigidBody* body)
{
	world->addCollisionObject(body);
}

// =============================================
void Physics3D::AddConstraintP2P(btRigidBody& bodyA, btRigidBody& bodyB, const float3& anchorA, const float3& anchorB)
{
	btTypedConstraint* p2p = new btPoint2PointConstraint(bodyA, bodyB, anchorA, anchorB);
	world->addConstraint(p2p);
	constraints.push_back(p2p);
	p2p->setDbgDrawSize(2.0f);
}

void Physics3D::AddConstraintHinge(btRigidBody& bodyA, btRigidBody& bodyB, const float3& anchorA, const float3& anchorB, const float3& axisA, const float3& axisB, bool disable_collision)
{
	btHingeConstraint* hinge = new btHingeConstraint(bodyA, bodyB, anchorA, anchorB, axisA, axisB);
	world->addConstraint(hinge, disable_collision);
	constraints.push_back(hinge);
	hinge->setDbgDrawSize(2.0f);
}
void Physics3D::SleepAllBodies()
{
	for (int i = 0; i < bodies.size(); i++)
	{
		bodies.at(i)->GetBody()->setActivationState(ISLAND_SLEEPING);
	}
}
void Physics3D::ActiveAllBodies()
{
	for (int i = 0; i < bodies.size(); i++)
	{
		if(bodies.at(i)->GetVehicle() != nullptr || bodies.at(i)->isKinematic)
			bodies.at(i)->GetBody()->setActivationState(DISABLE_DEACTIVATION);
		else
			bodies.at(i)->GetBody()->setActivationState(ACTIVE_TAG);
	}
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