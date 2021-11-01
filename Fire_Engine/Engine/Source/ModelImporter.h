#pragma once

class aiNode;
class aiMesh;
class aiScene;
class Texture;
class GameObject;

namespace ModelImporter
{
	void Import(const char* fullPath, char* buffer, int bufferSize, GameObject* root);
	void LoadMaterials(const aiScene* scene, const char* fullPath, std::vector<Texture*>& testTextures);
	
	void NodeToGameObject(aiMesh** meshArray, std::vector<Texture*>& sceneTextures, std::vector<Mesh*>& sceneMeshes, aiNode* node, GameObject* objParent, const char* holderName);
	void PopulateTransform(GameObject* child, aiNode* node);
}