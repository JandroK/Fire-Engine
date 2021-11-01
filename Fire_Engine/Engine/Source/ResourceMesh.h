#pragma once
#include "Resource.h"
//#include "MathGeoLib/include/Geometry/AABB.h"
//#include "MathGeoLib/include/Geometry/OBB.h"
//#include "MathGeoLib/include/Math/float3.h"
#include <vector>
#include "glmath.h"


typedef unsigned int uint;
typedef unsigned int GLuint;
//class vec3;


struct meshABC {

public:

	vec3 vecABC[3] = {  vec3(0, 0, 0), vec3(0, 0, 0), vec3(0, 0, 0) };
	vec3 A = vecABC[0];
	vec3 B = vecABC[1];
	vec3 C = vecABC[2];

	void UpdateABC() {
		A = vecABC[0];
		B = vecABC[1];
		C = vecABC[2];
	}
};


class Mesh : public Resource
{
public:
	Mesh();
	~Mesh();

	bool LoadToMemory() override;
	bool UnloadFromMemory() override;

	//void SetIndices(int indices[], int size);
	//void SetVertices(float vertices[], int size);
	//void SetTexCoords(float texCoords[], int size);

	void Render(GLuint textureID = -1);
	void EnableClientState();
	void DisableClientState();
	void UnBindBuffers(const GLuint& textureID);
	void DebugRender(bool* vertexNormals, bool* faceNormals);
	void RenderFaceNormals(float normalLenght);
	void RenderVertexNormals(float normalLenght);
	vec3 GetIndexVec(float* startValue);

public:
	// Buffers ID
	uint indexBufferId = 0;
	uint vertexBufferId = 0;
	uint textureBufferId = 0;
	uint normalBufferId = 0;
	// Counts
	uint numIndices = 0;
	uint numVertex = 0;
	uint numTexCoords = 0;
	uint numNormals = 0;
	// Vectors
	std::vector<uint> indexs;
	std::vector<float> vertex;
	std::vector<float> texCoords;
	std::vector<float> normals;

	//AABB* bbox;
	//OBB* obb;
};