#pragma once
#include "Component.h"
#include "Primitive.h"

class GameObject;
class btRigidBody;
struct PhysVehicle3D;

enum class CollisionType
{
	BOX = 0,
	SPHERE = 1,
	CAPSULE = 2,
	CYLINDER = 3,
	CONE = 4,
	STATIC_PLANE = 5,

	CAMERA = 6
};

class C_RigidBody : public Component
{
public:
	C_RigidBody(GameObject* obj, CollisionType type = CollisionType::BOX, float mass = 1.0f,  bool isKinematic = false);
	C_RigidBody();
	virtual ~C_RigidBody();

	void SetBoundingBox();
	void Update() override;
	void OnEditor() override;

	void Combos();

	void SetCollisionType(CollisionType type);
	void ResetLocalValues();
	void EditCollisionMesh();

	float4x4 btScalarTofloat4x4(btScalar* transform);

	void CreateBody();
	float GetMass() { return mass; };
	void SetMass(float mass);
	btRigidBody* GetBody() { return body; };
	PhysVehicle3D* GetVehicle() { return vehicle; };
	CollisionType GetCollisionType() { return collisionType; };

	void SetAsStatic();
	void SetAsVehicle();

	bool useGravity = true;
	bool isKinematic = false;
	std::vector<C_RigidBody*> constraintBodies;

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
	PhysVehicle3D* vehicle = nullptr;

	bool editMesh = false;

};
