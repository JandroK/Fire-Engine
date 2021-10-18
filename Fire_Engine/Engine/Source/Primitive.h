
#pragma once
#include "glmath.h"
#include "Color.h"
#include <vector>
#include "Mesh.h"

enum PrimitiveTypes
{
	Primitive_Point,
	Primitive_Line,
	Primitive_Plane,
	Primitive_Cube,
	Primitive_Sphere,
	Primitive_PCylinder
};

class Primitive : public Mesh
{
public:

	Primitive();

	virtual void	Render() const;
	virtual void	InnerRender() const;
	void			SetPos(float x, float y, float z);
	void			SetRotation(float angle, const vec3 &u);
	void			Scale(float x, float y, float z);
	PrimitiveTypes	GetType() const;

public:
	
	Color color;
	mat4x4 transform;
	bool axis,wire;

protected:
	PrimitiveTypes type;
};

// ============================================
class Cube : public Primitive
{
public :
	Cube();
	Cube(vec3 _size, vec3 pos);
	void InnerMesh();
public:
	vec3 size = { 0.5f, 0.5f, 0.5f };
};

// ============================================
class PSphere : public Primitive
{
public:
	PSphere();
	PSphere(float radius, int sectors, int stacks);
	void InnerMesh();

private:
	void SetVerticesMesh();
	void SetIndicesMesh();

public:
	float radius = 1;
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
	float radius = 1;
	float height = 3;
	int sectorCount = 24;

private:
	int baseCenterIndex;
	int topCenterIndex;
};


class Pyramid : public Primitive
{
public:
	Pyramid();
	Pyramid(float radius, float height, int sectorCount);
	void InnerMesh();

private:
	void SetVerticesMesh();
	void SetIndicesMesh();

public:
	float radius = 2;
	float height = 2;
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
	vec3 normal;
	float constant;
};