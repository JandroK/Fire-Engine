#include "Application.h"
#include "ModelImporter.h"
#include "Globals.h"

// Components
#include "GameObject.h"
#include "Component.h"
#include "MeshRenderer.h"
#include "Material.h"
#include "Transform.h"

// Importers
#include "FileSystem.h"
#include "MeshLoader.h"
#include "TextureLoader.h"

// Resources
#include "Resource.h"
#include "ResourceTexture.h"
#include "ResourceMesh.h"

#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#include "Assimp/include/cfileio.h"

#include"MathGeoLib/include/Math/Quat.h"

void ModelImporter::Import(const char* fullPath,  char* buffer, int bufferSize, GameObject* root)
{
	const aiScene* scene = aiImportFileFromMemory(buffer, bufferSize, aiProcessPreset_TargetRealtime_MaxQuality, nullptr);
	std::string fileName = StringLogic::FileNameFromPath(fullPath);

	// If the model has meshes continue, otherwise LogError
	if (scene != nullptr && scene->HasMeshes())
	{
		std::vector<Mesh*> modelMeshes;
		std::vector<Texture*> modelTextures;

		//Load all meshes into mesh vector
		LOG(LogType::L_NORMAL, "Loading meshes from %s", fileName);
		for (unsigned int i = 0; i < scene->mNumMeshes; i++)
		{
			modelMeshes.push_back(MeshLoader::LoadMesh(scene->mMeshes[i]));
		}

		// Load all materials
		LOG(LogType::L_NORMAL, "Loading materials from %s", fileName);
		LoadMaterials(scene, fullPath, modelTextures);

		// Load model as gameObject to scene root
		LOG(LogType::L_NORMAL, "Loading model as GameObject from %s", fileName);
		NodeToGameObject(scene->mMeshes, modelTextures, modelMeshes, scene->mRootNode, root, fileName.c_str());

		modelMeshes.clear();
		modelTextures.clear();

		aiReleaseImport(scene);
	}
	else
		LOG(LogType::L_ERROR, "Error loading model % s", fullPath);
}

void ModelImporter::LoadMaterials(const aiScene* scene, const char* fullPath, std::vector<Texture*>& testTextures)
{
	// Check if the model has materials
	if (scene->HasMaterials())
	{
		std::string generalPath(fullPath);
		generalPath = generalPath.substr(0, generalPath.find_last_of("/\\") + 1);
		for (size_t k = 0; k < scene->mNumMaterials; k++)
		{
			aiMaterial* material = scene->mMaterials[k];
			uint numTextures = material->GetTextureCount(aiTextureType_DIFFUSE);

			if (numTextures > 0)
			{
				aiString path;
				material->GetTexture(aiTextureType_DIFFUSE, 0, &path);

				std::string localPath = StringLogic::GlobalToLocalPath(fullPath);
				localPath = localPath.substr(0, localPath.find_last_of('/') + 1);

				std::string fileNamePath = FileSystem::NormalizePath(path.C_Str());
				fileNamePath = fileNamePath.substr(fileNamePath.find_last_of("/\\") + 1);

				localPath += fileNamePath;

				char* buffer = nullptr;
				int size = FileSystem::LoadToBuffer(localPath.c_str(), &buffer);

				if (buffer != nullptr)
				{
					Texture* tex = new Texture(localPath.c_str());
					tex->LoadToMemory();

					testTextures.push_back(tex);

					RELEASE_ARRAY(buffer)
				}

				path.Clear();
			}
		}
	}
}

void ModelImporter::NodeToGameObject(aiMesh** meshArray, std::vector<Texture*>& sceneTextures, std::vector<Mesh*>& sceneMeshes, aiNode* node, GameObject* objParent, const char* holderName)
{
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		Mesh* meshPointer = sceneMeshes[node->mMeshes[i]];

		GameObject* gmNode = new GameObject(node->mName.C_Str());
		gmNode->SetParent(objParent);

		//Load mesh to GameObject
		MeshRenderer* gmMeshRenderer = dynamic_cast<MeshRenderer*>(gmNode->AddComponent(ComponentType::MESHRENDERER));

		gmMeshRenderer->SetMesh(meshPointer);

		aiMesh* importedMesh = meshArray[node->mMeshes[i]];
		if (importedMesh->mMaterialIndex < sceneTextures.size())
		{
			Material* material = dynamic_cast<Material*>(gmNode->AddComponent(ComponentType::MATERIAL));
			material->texture = sceneTextures[importedMesh->mMaterialIndex];
		}

		PopulateTransform(gmNode, node);
		objParent->AddChildren(gmNode);
	}

	if (node->mNumChildren > 0)
	{
		GameObject* rootGO = objParent;

		if (node->mNumChildren == 1 && node->mParent == nullptr && node->mChildren[0]->mNumChildren == 0)
		{
			LOG(LogType::L_WARNING, "This is a 1 child gameObject, you could ignore the root node parent creation");
			node->mChildren[0]->mName = holderName;
		}
		else
		{
			rootGO = new GameObject(holderName);
			rootGO->SetParent(objParent);
			PopulateTransform(rootGO, node);
			objParent->AddChildren(rootGO);
		}


		for (unsigned int i = 0; i < node->mNumChildren; i++)
		{
			NodeToGameObject(meshArray, sceneTextures, sceneMeshes, node->mChildren[i], rootGO, node->mChildren[i]->mName.C_Str());
		}
	}
}

void ModelImporter::PopulateTransform(GameObject* child, aiNode* node)
{
	aiVector3D translation, scaling;
	aiQuaternion rotation;

	node->mTransformation.Decompose(scaling, rotation, translation);

	float3 pos(translation.x, translation.y, translation.z);
	float3 scale(scaling.x, scaling.y, scaling.z);
	Quat rot(rotation.x, rotation.y, rotation.z, rotation.w);

	child->transform->SetTransformMatrix(pos, rot, scale, child->GetParent()->transform);
}
