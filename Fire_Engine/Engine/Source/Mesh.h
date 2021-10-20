#pragma once

#include "Globals.h"
#include <vector>

class Mesh
{
public:
	Mesh();

	void SetupMesh();
	void SetIndices(int _indices[], int size);
	void SetVertices(float _vertices[], int size);
	void SetTexCoords(float _texCoords[], int size);

	void Draw();
	void CleanUp();

public:
	// mesh data
	uint indexBufferId = 0;
	uint vertexBufferId = 0;
	uint textureBufferId = 0;
	uint numIndices = 0;
	std::vector<uint> indices;
	std::vector<float> vertices;
	std::vector<float> texCoords;


};