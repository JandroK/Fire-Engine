#pragma once
#include "Component.h"

#include"Math/float4x4.h"
#include"Math/float3.h"
#include"Math/Quat.h"
#include "glmath.h"

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

	float3 GetLocalPosition() { return position; };
	float3 GetWorldPosition() { return worldPosition; };
	Quat GetWorldRotation() { return worldRotation; };
	float3 GetWorldEulerRotation() { return worldEulerRotation; };
	float3 GetLocalScale() { return scale; };
	float3 GetWorldScale() { return worldScale; };

	void SetPosition(float3 pos) { localTransform.SetCol3(3, pos); position = pos; };
	void SetWorldPosition(float3 pos) { globalTransform.SetCol3(3, pos); worldPosition = pos; };
	void SetEulerRotation(float3 rot) { localTransform.SetRotatePart(Quat::FromEulerXYZ(rot.x * DEGTORAD, rot.y * DEGTORAD, rot.z * DEGTORAD)); };
	void SetWorldRotation(Quat rot) { globalTransform.SetRotatePart(rot); worldRotation = rot; worldEulerRotation = rot.ToEulerXYZ();};
	void SetUpdateTransform(bool ret) { updateTransform = ret; };

	void SetTransformMatrix(float3 position, Quat rotation, float3 localScale, Transform* parent);
	void SetTransformMFromM(float4x4 matrix);


	void SetLocalTransform(float4x4 matrix) { localTransform = matrix; };
	void SetGlobalTransform(float4x4 matrix) { globalTransform = matrix; };

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

	mat4x4 float4x4ToMat4x4();

	void operator=(Transform*);

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