#pragma once

#include "Globals.h"
#include <vector>
#include "Math/float3.h"



class Mesh
{
public:
	Mesh();

	void SetupMesh();
	void SetIndices(int _indices[], int size);
	void SetVertices(std::vector<float3> _vertices, int size);

	void Draw();

public:
	// mesh data
	uint indexBufferId = 0;
	uint vertexBufferId = 0;
	uint numIndices = 0;
	uint numVertex = 0;
	uint numNormals = 0;
	std::vector<uint> meshIndex;
	std::vector<float3> meshVertex;
	std::vector<float3> meshNormals;

};