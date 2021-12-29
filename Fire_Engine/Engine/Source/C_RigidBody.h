#pragma once
#include "Component.h"
#include "Geometry/GeometryAll.h"

class GameObject;
class btRigidBody;

enum class CollisionType
{
	BOX,
	SPHERE,
	CAPSULE,
	CYLINDER,
	CONE,
	STATIC_PLANE
};

class C_RigidBody : public Component
{
public:
	C_RigidBody(GameObject* obj);
	virtual ~C_RigidBody();

	void OnEditor() override;

	void CreateBody();
	float GetMass() { return mass; };

private:
	btRigidBody* body = nullptr;
	CollisionType collisionType = CollisionType::BOX;

	float mass = 1.0f;
	float friction = 1.0f;
	float restitution = 1.0f;

	// Block the movement and rotation
	float3 movementConstraint = float3::one;
	float3 rotationConstraint = float3::one;

	// Having linear damping at zero means objects will keep moving until friction slows them down.
	float linearDamping = 0.0f;
	float angularDamping = 0.0f;

	bool useGravity = true;
	bool isKinematic = false;

	// Figures
	OBB box;
	Sphere sphere;
	Capsule capsule;
	Cylinder cylinder;
	Cone cone;
	Plane plane;
};
