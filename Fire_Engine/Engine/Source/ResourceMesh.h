#pragma once

#include "Globals.h"
#include <vector>
#include "Resource.h"

typedef unsigned int GLuint;

class Mesh : public Resource
{
public:
	Mesh(unsigned int _uid);
	~Mesh();

	bool LoadToMemory() override;
	bool UnloadFromMemory() override;

	void SetIndices(int _indices[], int size);
	void SetVertices(float _vertices[], int size);
	void SetTexCoords(float _texCoords[], int size);

	void RenderMesh(GLuint textureID);
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