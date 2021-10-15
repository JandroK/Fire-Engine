#pragma once

#include "Globals.h"
#include <vector>

class Mesh
{
public:
	Mesh();

	void Draw();
	void SetupMesh();

public:
	// mesh data
	uint indexBufferId = 0;
	uint vertexBufferId = 0;
	uint numIndices = 0;
	std::vector<uint> indices;
	std::vector<float> vertices;

};