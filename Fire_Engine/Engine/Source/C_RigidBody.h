#pragma once
#include "Component.h"

class GameObject;
class btRigidBody;

class C_RigidBody : public Component
{
public:
	C_RigidBody(GameObject* obj);
	virtual ~C_RigidBody();

	void OnEditor() override;

private:
	btRigidBody* body = nullptr;
	float mass = 1.0f;

	// Block the movement and rotation
	float3 movementConstraint = float3::one;
	float3 rotationConstraint = float3::one;

	bool useGravity = true;
	bool isKinematic = false;
};
