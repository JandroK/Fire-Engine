#include "Application.h"
#include "MeshLoader.h"

#include "Editor.h"

// Importers
#include "FileSystem.h"
#include "TextureLoader.h"

// Resources
#include "ResourceMesh.h"
#include "ResourceTexture.h"

#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#include "Assimp/include/cfileio.h"

void MeshLoader::LogCallback(const char* message, char* user)
{
	app->editor->LogToConsole(message, LogType::L_NORMAL);
}
void MeshLoader::EnableDebugMode()
{
	struct aiLogStream stream;
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
	stream.callback = LogCallback;
	aiAttachLogStream(&stream);
}
void MeshLoader::DisableDebugMode()
{
	// Detach log stream
	aiDetachAllLogStreams();
}

Mesh* MeshLoader::LoadMesh(aiMesh* importedMesh)
{
	LOG(LogType::L_NORMAL, "%s", importedMesh->mName.C_Str());

	Mesh* mesh = new Mesh();

	std::string fileName = MESHES_FOLDER;
	fileName += importedMesh->mName.C_Str();
	fileName += mesh->GetFormatExt();
	mesh->SetLibraryPath(fileName.c_str());

	if (FileSystem::Exists(fileName.c_str()))
	{
		mesh->LoadFromFME(fileName.c_str());
		LOG(LogType::L_NORMAL, "Loaded mesh %s from library path", importedMesh->mName.C_Str());
		LOG(LogType::L_NORMAL, "%d vertices", mesh->numVertex);
		LOG(LogType::L_NORMAL, "%d normals", mesh->numNormals);
		LOG(LogType::L_NORMAL, "%d texture coords", mesh->numTexCoords);
	}
	else
	{

		// copy vertices
		mesh->numVertex = importedMesh->mNumVertices;
		mesh->numTexCoords = importedMesh->mNumVertices;
		mesh->numNormals = importedMesh->mNumVertices;

		for (size_t i = 0; i < mesh->numVertex; i++)
		{
			mesh->vertex.push_back(importedMesh->mVertices[i].x);
			mesh->vertex.push_back(importedMesh->mVertices[i].y);
			mesh->vertex.push_back(importedMesh->mVertices[i].z);

			if (importedMesh->HasNormals())
			{
				mesh->normals.push_back(importedMesh->mNormals[i].x);
				mesh->normals.push_back(importedMesh->mNormals[i].y);
				mesh->normals.push_back(importedMesh->mNormals[i].z);
			}
			if (importedMesh->HasTextureCoords(0))
			{
				mesh->texCoords.push_back(importedMesh->mTextureCoords[0][i].x);
				mesh->texCoords.push_back(importedMesh->mTextureCoords[0][i].y);
			}
			else mesh->numTexCoords = 0;
		}

		importedMesh->HasVertexColors(0);

		// Generate indices
		if (importedMesh->HasFaces())
		{
			mesh->numIndexs = importedMesh->mNumFaces * 3;
			mesh->indexs.resize(mesh->numIndexs);

			for (uint j = 0; j < importedMesh->mNumFaces; ++j)
			{
				if (importedMesh->mFaces[j].mNumIndices != 3)
				{
					LOG(LogType::L_WARNING, "WARNING, geometry face with != 3 indices!");
				}
				else
				{
					memcpy(&mesh->indexs[j * 3], importedMesh->mFaces[j].mIndices, 3 * sizeof(uint));
				}
			}
		}

		// Custom File Format
		uint size = 0;
		char* buffer = mesh->SaveToFME(size);
		FileSystem::Save(fileName.c_str(), buffer, size);

		RELEASE_ARRAY(buffer);

		mesh->LoadToMemory();
		mesh->GenerateBounds();

		LOG(LogType::L_NORMAL, "New mesh with %d vertices", mesh->numVertex);
		LOG(LogType::L_NORMAL, "New mesh with %d normals", mesh->numNormals);
		LOG(LogType::L_NORMAL, "New mesh with %d texture coords", mesh->numTexCoords);
	}

	return mesh;
}