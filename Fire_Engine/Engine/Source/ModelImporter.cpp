#include "Application.h"
#include "ModelImporter.h"
#include "Globals.h"

#include "Resource.h"
#include "ResourceTexture.h"
#include "ResourceMesh.h"

#include "FileSystem.h"
#include "MeshLoader.h"

#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#include "Assimp/include/cfileio.h"


void ModelImporter::Import(char* buffer, int bSize, Resource* res)
{
	const aiScene* scene = aiImportFileFromMemory(buffer, bSize, aiProcessPreset_TargetRealtime_MaxQuality, nullptr);

	if (scene != nullptr && scene->HasMeshes())
	{
		std::vector<Mesh*> meshesOnModelUIDs;

		//Load mesh uid from meta file
		std::vector<uint> uids;
		//if (FileSystem::Exists(app->resources->GetMetaPath(res->GetAssetPath()).c_str()))
			//GetMeshesFromMeta(res->GetAssetPath(), uids);

		if (scene->HasMeshes())
		{
			for (unsigned int i = 0; i < scene->mNumMeshes; i++)
			{
				if (uids.size() != 0)
					meshesOnModelUIDs.push_back(MeshLoader::LoadMesh(scene->mMeshes[i], uids[i]));
				else
					meshesOnModelUIDs.push_back(MeshLoader::LoadMesh(scene->mMeshes[i]));
			}
		}

		meshesOnModelUIDs.clear();
		uids.clear();

		aiReleaseImport(scene);
	}
	else
		LOG(LogType::L_ERROR, "Error loading scene"/*, scene->name*/);
}