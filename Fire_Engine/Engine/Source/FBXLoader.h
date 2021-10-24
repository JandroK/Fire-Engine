#pragma once

#include <vector>
#include"MathGeoLib/include/Math/Quat.h"

class Mesh;
class aiNode;
class aiMesh;
class Texture;
typedef unsigned int uint;

namespace MeshLoader
{
	void EnableDebugMode();
	void DisableDebugMode();
	void LogCallback(const char* message, char* user);

	Mesh* LoadMesh(aiMesh* importedMesh, uint oldUID = 0);
}