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

	float3 linearFactor = float3::one;
	float3 angularFactor = float3::one;

	bool useGravity = true;
	bool isKinematic = false;
};
