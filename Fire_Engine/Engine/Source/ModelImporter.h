#pragma once
#include "Math/float3.h"
#include "Math/Quat.h"
#include "Assimp/include/cfileio.h"

class aiNode;
class aiMesh;
class aiScene;
class Texture;
class GameObject;
class Mesh;

struct ConversionF
{
	float3 pos;
	float3 scale;
	Quat rot;
	aiVector3D posAi;
	aiVector3D scaleAi;
	aiQuaternion rotAi;

	void AssimpToFloat()
	{
		pos = { posAi.x, posAi.y, posAi.z };
		scale = { scaleAi.x, scaleAi.y, scaleAi.z };
		rot = { rotAi.x, rotAi.y, rotAi.z, rotAi.w };
	}
	void AcumulativeTransform()
	{
		pos += float3(posAi.x, posAi.y, posAi.z);
		scale = float3(scale.x * scaleAi.x, scale.y * scaleAi.y, scale.z * scaleAi.z);
		rot = rot * Quat(rotAi.x, rotAi.y, rotAi.z, rotAi.w);
	}
};

namespace ModelImporter
{
	void Import(const char* fullPath, char* buffer, int bufferSize, GameObject* root);
	void LoadMaterials(const aiScene* scene, const char* fullPath, std::vector<Texture*>& testTextures);
	
	void NodeToGameObject(aiMesh** meshArray, std::vector<Texture*>& sceneTextures, std::vector<Mesh*>& sceneMeshes, aiNode* node, GameObject* objParent, const char* ownerName);
	void CalculateAcumulativeTransformation(aiNode*& node, ConversionF& con);
	void FillGameObject(aiNode* node, std::vector<Mesh*>& sceneMeshes, GameObject* objParent, aiMesh** meshArray, std::vector<Texture*>& sceneTextures, ConversionF con);
	
	void PopulateTransform(GameObject* child, ConversionF con);
}