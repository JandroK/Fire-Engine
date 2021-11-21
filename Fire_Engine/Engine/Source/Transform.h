#pragma once
#include "Component.h"

#include"Math/float4x4.h"
#include"Math/float3.h"
#include"Math/Quat.h"

class Transform : public Component
{
public:
	Transform(GameObject* obj);
	virtual ~Transform() {};

	void Update() override;
	void OnEditor() override;

	void CheckStateMode();

	void CheckStateOperation();

	float3 GetPosition() { return position; };
	void SetPosition(float3 pos) { position = pos; };
	void SetEulerRotaion(float3 rot) { eulerRotation = rot; };

	void SetTransformMatrix(float3 position, Quat rotation, float3 localScale, Transform* parent);
	void SetTransformMFromGlobalM(float4x4 globalMatrix);

	float4x4 GetGlobalTransform() {return globalTransform;};
	float4x4 GetGlobalTransformT() { return globalTransformTransposed; };

	void UpdateTransform();
	void NewAttachment();
	void ResetTransform();

	void UpdateBoundingBoxes();

	float3 GetForward();
	float3 GetUp();
	float3 GetRight();
	float3 GetNormalizeAxis(int i);

private:

	Transform* GetRecursiveTransforms(Transform* node, std::vector<Transform*>& transforms);

	bool updateTransform = false;

	// Local transforms
	float4x4 localTransform;
	float3 position, scale = { 1,1,1 };
	Quat rotation = Quat::identity; // Quat is best than float3
	float3 eulerRotation = { 0,0,0 }; // Need a container that holds the conversion of Quad to float3 for draw it on OnEditor

	// Globals transforms
	float4x4 globalTransform;
	float4x4 globalTransformTransposed;
};