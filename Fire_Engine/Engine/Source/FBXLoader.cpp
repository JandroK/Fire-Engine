#include "FBXLoader.h"
#include "Globals.h"

#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"


//#include "mmgr/mmgr.h"

FBXLoader::FBXLoader()
{
}

FBXLoader::~FBXLoader()
{
}

void FBXLoader::LoadMesh(const char* path)
{
	const aiScene* scene = aiImportFile(path, aiProcessPreset_TargetRealtime_MaxQuality);
	if (scene != nullptr && scene->HasMeshes())
	{
		for (int i = 0; i < scene->mNumMeshes; ++i)
		{
			aiMesh* mesh = scene->mMeshes[i];
			ourMesh.numVertex = mesh->mNumVertices;
			

			//Se comporta como el new, lo guarda en memoria todo seguido, redimensionando el array a un espacio de float3
			ourMesh.meshVertex.resize(sizeof(float3) * mesh->mNumVertices);
			//El memcopy copia la memoria al destino que tu le destino 
			// memcpy(destino, origen, cantidad de bytes que desas copiar)
			memcpy(&ourMesh.meshVertex[0], mesh->mVertices, sizeof(float3) * mesh->mNumVertices);

			LOG(LogType::L_NORMAL,"New mesh with %d vertices", mesh->mNumVertices);

			if (mesh->HasFaces())
			{
				ourMesh.numIndices = mesh->mNumFaces * 3;
				ourMesh.meshIndex.resize(ourMesh.numIndices);
				
				for (unsigned int j = 0; j < mesh->mNumFaces; ++j)
				{
					aiFace& face = mesh->mFaces[j];
					if (face.mNumIndices != 3) 
					{

						LOG(LogType::L_WARNING, "WARNING, geometry face with != 3 indices!");
					}
					else 
					{
						// Guarda vectores de 3 en 3, es decir los poligonos en forma triangular
						memcpy(&ourMesh.meshIndex[j * 3], face.mIndices, 3 * sizeof(uint));
					}

				}
			}

			if (mesh->HasNormals())
			{
				ourMesh.numNormals = mesh->mNumVertices;
				//ourMesh.meshNormals = new float3[ourMesh.numNormals];
				ourMesh.meshNormals.resize(ourMesh.numNormals);
				memcpy(&ourMesh.meshNormals, mesh->mNormals, sizeof(float3) * ourMesh.numNormals);
			}
		}
		// Use scene->mNumMeshes to iterate on scene->mMeshes array
		aiReleaseImport(scene);
	}
	else
		LOG(LogType::L_ERROR, "Error loading scene % s", path);
}

bool FBXLoader::CleanUp() 
{
	aiDetachAllLogStreams();
	return true;
}