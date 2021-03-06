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
#include "ResourceManager.h"

#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"

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
		LOG(LogType::L_NORMAL, "Loading meshes from %c", fileName.c_str());
		for (unsigned int i = 0; i < scene->mNumMeshes; i++)
		{
			modelMeshes.push_back(MeshLoader::LoadMesh(scene->mMeshes[i]));
			modelMeshes.at(i)->SetAssetsPath(fullPath);
		}

		// Load all materials
		LOG(LogType::L_NORMAL, "Loading materials from %s", fileName.c_str());
		LoadMaterials(scene, fullPath, modelTextures);

		// Load model as gameObject to scene root
		LOG(LogType::L_NORMAL, "Loading model as GameObject from %s", fileName.c_str());
		NodeToGameObject(scene->mMeshes, modelTextures, modelMeshes, scene->mRootNode, root, fileName.c_str());

		// Clear
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
			// At the moment only take the diffuse type textures 
			uint numTextures = material->GetTextureCount(aiTextureType_DIFFUSE);

			// Check if model has textures, because it can have material but not texture 
			if (numTextures > 0)
			{
				aiString path;
				material->GetTexture(aiTextureType_DIFFUSE, 0, &path);

				std::string localPath(fullPath);
				localPath = localPath.substr(0, localPath.find_last_of('/') + 1);

				std::string fileNamePath = FileSystem::NormalizePath(path.C_Str());
				fileNamePath = fileNamePath.substr(fileNamePath.find_last_of("/\\") + 1);

				// localPath = from output directory until the end 
				localPath += fileNamePath;

				// Get the buffer
				char* buffer = nullptr;
				uint size = FileSystem::LoadToBuffer(localPath.c_str(), &buffer);

				if (buffer != nullptr)
				{
					// Load texture
					Texture* tex = new Texture(localPath.c_str());
					if(!FileSystem::Exists(tex->GetLibraryPath())) tex->Import(buffer, size, tex->GetLibraryPath());
					tex->LoadToMemory();

					testTextures.push_back(tex);

					RELEASE_ARRAY(buffer)
				}

				path.Clear();
			}
			else
				testTextures.push_back(app->resourceManager->defaultTexture);
		}
	}
}

void ModelImporter::NodeToGameObject(aiMesh** meshArray, std::vector<Texture*>& sceneTextures, std::vector<Mesh*>& sceneMeshes, aiNode* node, GameObject* objParent, const char* ownerName)
{
	ConversionF con;

	CalculateAcumulativeTransformation(node, con);
	if(node->mNumMeshes > 0)
		FillGameObject(node, sceneMeshes, objParent, meshArray, sceneTextures, con);

	if (node->mNumChildren > 0)
	{
		GameObject* rootGO = objParent;
		// The models that are only a mesh and these do not have children,
		// They are added to the hierarchy as children of root
		// Instead of creating an empty object that does the group function
		if (node->mNumChildren == 1 && node->mParent == nullptr && node->mChildren[0]->mNumChildren == 0)
		{
			node->mChildren[0]->mName = ownerName;
		}
		else
		{
			// Connect the new game object with its father
			rootGO = new GameObject(ownerName);
			// Set parent (connect object with father)
			rootGO->SetParent(objParent);
			// Loading transformation
			PopulateTransform(rootGO, con);
			// Add Child (connect father with object) 
			objParent->AddChildren(rootGO);
		}
		// Recursive
		for (unsigned int i = 0; i < node->mNumChildren; i++)
		{
			NodeToGameObject(meshArray, sceneTextures, sceneMeshes, node->mChildren[i], rootGO, node->mChildren[i]->mName.C_Str());
		}
	}
}

void ModelImporter::CalculateAcumulativeTransformation(aiNode*& node, ConversionF& con)
{
	//Decomposing transform matrix into translation rotation and scale
	node->mTransformation.Decompose(con.scaleAi, con.rotAi, con.posAi);
	con.AssimpToFloat();

	std::string nodeName = node->mName.C_Str();
	bool dummyFound = true;
	while (dummyFound)
	{
		dummyFound = false;

		//All dummy modules have one children (next dummy module or last module containing the mesh)
		if (node->mMetaData == nullptr && node->mNumChildren == 1)
		{
			//Dummy module have only one child node, so we use that one as our next node
			node = node->mChildren[0];

			// Accumulate transform 
			node->mTransformation.Decompose(con.scaleAi, con.rotAi, con.posAi);
			con.AcumulativeTransform();

			nodeName = node->mName.C_Str();
			dummyFound = true;
		}
	}
}

void ModelImporter::FillGameObject(aiNode* node, std::vector<Mesh*>& sceneMeshes, GameObject* objParent, aiMesh** meshArray, std::vector<Texture*>& sceneTextures, ConversionF con)
{
	// Create new GameObject, will fill it and set his parent and add him as son 
	GameObject* gmEmpty = new GameObject(node->mName.C_Str());
	gmEmpty->SetParent(objParent);

	// Add mesh component to GameObject and fill it
	MeshRenderer* gmMeshRenderer = static_cast<MeshRenderer*>(gmEmpty->AddComponent(ComponentType::MESHRENDERER));
	gmMeshRenderer->SetMesh(sceneMeshes[node->mMeshes[0]]);

	// Fill each mesh with its respective texture 
	aiMesh* importedMesh = meshArray[node->mMeshes[0]];
	if (importedMesh->mMaterialIndex < sceneTextures.size())
	{
		Material* material = static_cast<Material*>(gmEmpty->AddComponent(ComponentType::MATERIAL));
		material->texture = sceneTextures[importedMesh->mMaterialIndex];
	}
	// Loading transformation
	PopulateTransform(gmEmpty, con);
	objParent->AddChildren(gmEmpty);
	gmMeshRenderer->SetBoundingBoxes(sceneMeshes[node->mMeshes[0]]);

}

void ModelImporter::PopulateTransform(GameObject* child, ConversionF con)
{
	// Break transformation info in position, scaleand rotation
	child->transform->SetTransformMatrix(con.pos, con.rot, con.scale, child->GetParent()->transform);
}
