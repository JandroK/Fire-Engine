#pragma once
#include "Resource.h"
#include <vector>

typedef unsigned int uint;
typedef unsigned int GLuint;

class Mesh : public Resource
{
public:
	Mesh(unsigned int _uid);
	~Mesh();

	bool LoadToMemory() override;
	bool UnloadFromMemory() override;

	void SetIndices(int indices[], int size);
	void SetVertices(float vertices[], int size);
	void SetTexCoords(float texCoords[], int size);

	void RenderMesh(GLuint textureID = -1);
	void CleanUp();

public:
	// Buffers ID
	uint indexBufferId = 0;
	uint vertexBufferId = 0;
	uint textureBufferId = 0;
	// Counts
	uint numIndices = 0;
	uint numVertex = 0;
	uint numTexCoords = 0;
	// Vectors
	std::vector<uint> indices;
	std::vector<float> vertices;
	std::vector<float> texCoords;
};