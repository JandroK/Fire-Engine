
#pragma once
#include "glmath.h"
#include "Color.h"
#include <vector>
#include "ResourceMesh.h"

enum PrimitiveTypes
{
	Primitive_Point,
	Primitive_Line,
	Primitive_Plane,
	Primitive_Cube,
	Primitive_Sphere,
	Primitive_Cylinder,
	Primitive_Pyramid,
};

class Primitive
{
public:

	Primitive();
	~Primitive() {};

	virtual void	Render() const;
	virtual void	InnerRender() const;
	void			SetPos(float x, float y, float z);
	void			SetPos(float3 pos);
	void			SetRotation(float angle, const vec3 &u);
	void			SetScale(float x, float y, float z);
	void			SetScale(float3 scale);

	void SetVertices(float vertices[], int size);
	void SetTexCoords(float texCoords[], int size);
	void SetIndices(int indices[], int size);

	PrimitiveTypes	GetType() const;

public:
	
	Color color;
	mat4x4 transform;
	bool axis,wire;

	Mesh* mesh;

protected:
	PrimitiveTypes type;
};

// ============================================
class PCube : public Primitive
{
public :
	PCube();
	PCube(float3 _size, float3 pos);
	PCube(float sizeX, float sizeY, float sizeZ);
	void InnerMesh();
public:
	float3 size = {1, 1, 1 };
};

// ============================================
class PSphere : public Primitive
{
public:
	PSphere();
	PSphere(float radius);
	PSphere(float radius, int sectors, int stacks);
	void InnerMesh();

private:
	void SetVerticesMesh();
	void SetIndicesMesh();

public:
	float radius = 0.5f;
	int sectors = 36;
	int stacks = 18;
};

// ============================================
class PCylinder : public Primitive
{
public:
	PCylinder();
	PCylinder(float radius, float height, int sectorCount);
	void InnerMesh();

private:
	std::vector<float> GetUnitCircleVertices();
	void SetVerticesMesh();
	void SetIndicesMesh();

public:
	float radius = 0.5f;
	float height = 1;
	int sectorCount = 24;

private:
	int baseCenterIndex;
	int topCenterIndex;
};


class PPyramid : public Primitive
{
public:
	PPyramid();
	PPyramid(float radius, float height, int sectorCount);
	void InnerMesh();

public:
	float radius = 0.5f;
	float height = 1;
};

// ============================================
class PLine : public Primitive
{
public:
	PLine();
	PLine(float x, float y, float z);
	void InnerRender() const;
public:
	vec3 origin;
	vec3 destination;
};

// ============================================
class PPlane : public Primitive
{
public:
	PPlane();
	PPlane(float x, float y, float z, float d);
	void InnerRender() const;
public:
	float3 normal;
	float constant;
};