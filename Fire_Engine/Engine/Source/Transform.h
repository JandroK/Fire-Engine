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

	void EditTransform(float4x4 &trans, float3 &pos, Quat &rot, float3 &euler, float3 &scale);

	void CheckStateMode();

	void CheckStateOperation();

	float3 GetPosition() { return position; };
	void SetPosition(float3 pos) { position = pos; };
	void SetEulerRotaion(float3 rot) { eulerRotation = rot; };

	void SetTransformMatrix(float3 position, Quat rotation, float3 localScale, Transform* parent);
	void SetTransformMFromM(float4x4 matrix);

	float4x4 GetGlobalTransform() {return globalTransform;};
	float4x4 GetGlobalTransformT() { return globalTransformTransposed; };
	float4x4 GetLocalTransform() { return localTransform; };

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
	float4x4 localTransform; //
	float3 position = { 0,0,0 }, scale = { 1,1,1 };
	Quat rotation = Quat::identity; // Quat is best than float3
	float3 eulerRotation = { 0,0,0 }; // Need a container that holds the conversion of Quad to float3 for draw it on OnEditor

	// World transforms
	float3 worldPosition = { 0,0,0 }, worldScale = { 1,1,1 };
	Quat worldRotation = Quat::identity; // Quat is best than float3
	float3 worldEulerRotation = { 0,0,0 }; // Need a container that holds the conversion of Quad to float3 for draw it on OnEditor

	// Globals transforms
	float4x4 globalTransform; //
	float4x4 globalTransformTransposed;
};