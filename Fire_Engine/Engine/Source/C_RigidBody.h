#pragma once
#include "Component.h"
#include "Primitive.h"

class GameObject;
class btRigidBody;

enum class CollisionType
{
	BOX,
	SPHERE,
	CAPSULE,
	CYLINDER,
	CONE,
	STATIC_PLANE,

	CAMERA
};

class C_RigidBody : public Component
{
public:
	C_RigidBody(GameObject* obj, float mass = 1.0f, CollisionType type = CollisionType::BOX);
	C_RigidBody();
	virtual ~C_RigidBody();

	void SetBoundingBox();
	void Update() override;
	void OnEditor() override;

	void SetCollisionType(CollisionType type);
	void ResetLocalValues();
	void EditCollisionMesh();

	float4x4 btScalarTofloat4x4(btScalar* transform);

	void CreateBody();
	float GetMass() { return mass; };
	btRigidBody* GetBody() { return body; };

	void SetAsStatic();

	bool useGravity = true;
	bool isKinematic = false;

private:
	btRigidBody* body = nullptr;
	CollisionType collisionType = CollisionType::BOX;

	float mass = 1.0f;
	float friction = 1.0f;
	float restitution = 0.2f;
	float3 offset = { 0,0,0 };

	// Block the movement and rotation
	float3 movementConstraint = float3::one;
	float3 rotationConstraint = float3::one;

	// Having linear damping at zero means objects will keep moving until friction slows them down.
	float linearDamping = 0.0f;
	float angularDamping = 0.0f;

	// Figures
	PCube box;
	PSphere sphere;
	PCapsule capsule;
	PCylinder cylinder;
	PPyramid cone;
	PPlane plane;

	bool editMesh = false;
};
