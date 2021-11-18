#pragma once
#include "Component.h"
#include "Geometry/Frustum.h"

class ComponentCamera : public Component
{
public:
	ComponentCamera(GameObject* obj);
	virtual ~ComponentCamera() {};

	void Update() override;
	void OnEditor() override;

	Frustum frustrum;
	bool showFrustrum = false;
};