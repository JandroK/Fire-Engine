#pragma once
#include "Component.h"
#include "Primitive.h"

class GameObject;
class btRigidBody;

enum class CollisionType
{
	BOX,
	SPHERE,
	//CAPSULE,
	CYLINDER,
	PYRAMID,
	STATIC_PLANE
};

class C_RigidBody : public Component
{
public:
	C_RigidBody(GameObject* obj);
	virtual ~C_RigidBody();

	void SetBoundingBox(OBB& bbox);
	void Update() override;
	void OnEditor() override;

	void CreateBody();
	float GetMass() { return mass; };

	void OnDebugDraw() const;

private:
	btRigidBody* body = nullptr;
	CollisionType collisionType = CollisionType::BOX;

	float mass = 1.0f;
	float friction = 1.0f;
	float restitution = 1.0f;
	bool draw = true;

	// Block the movement and rotation
	float3 movementConstraint = float3::one;
	float3 rotationConstraint = float3::one;

	// Having linear damping at zero means objects will keep moving until friction slows them down.
	float linearDamping = 0.0f;
	float angularDamping = 0.0f;

	bool useGravity = true;
	bool isKinematic = false;

	// Figures
	PCube box;
	PSphere sphere;
	//PCapsule capsule;
	PCylinder cylinder;
	PPyramid pyramid;
	PPlane plane;
};
