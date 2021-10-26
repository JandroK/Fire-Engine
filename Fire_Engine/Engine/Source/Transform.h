#pragma once
#include "Component.h"

#include"MathGeoLib/include/Math/float4x4.h"
#include"MathGeoLib/include/Math/float3.h"
#include"MathGeoLib/include/Math/Quat.h"

class Transform : public Component
{
public:
	Transform(GameObject* obj);
	virtual ~Transform();

	void Update() override;
	void OnEditor() override;

	float4x4 globalTransform;
	float4x4 localTransform;

	float3 position, localScale, eulerRotation;
	Quat rotation;

	bool updateTransform;

	void SetTransformMatrix(float3 position, Quat rotation, float3 localScale, Transform* parent);
	float* GetGlobalTransposed() {return globalTransformTransposed.ptr();};

private:
	void UpdateTransform();
	Transform* GetRecursiveTransforms(Transform* node, std::vector<Transform*>& transforms);

	float4x4 globalTransformTransposed;
};