#include "Application.h"
#include "ResourceManager.h"

//Importers
#include "FileSystem.h"
#include "TextureLoader.h"
#include "MeshLoader.h"
#include "ModelImporter.h"

//Resources
#include "ResourceTexture.h"
#include "ResourceMesh.h"

//Tabs
#include "Inspector.h" 

//Components
#include "Material.h"

ResourceManager::ResourceManager(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	name = "ResourceManager";	
}

ResourceManager::~ResourceManager()
{
}

bool ResourceManager::Init()
{	
	FileSystem::FSInit();	

	return true;
}

bool ResourceManager::Start()
{
	// Import Icon
	logo = new Texture("Assets/logo.png", "logo");
	logo->LoadToMemory();

	app->renderer3D->globalTextures.push_back(logo);

	return true;
}

bool ResourceManager::CleanUp()
{
	FileSystem::FSDeInit();
	return true;
}

void ResourceManager::ImportFile(const char* assetsFile)
{
	std::string normalizedPath = FileSystem::NormalizePath(assetsFile);
	std::string relativePath = StringLogic::GlobalToLocalPath(assetsFile);

	//Duplicate file
	//BUG: This will only allow to work with files inside PhysFS dir
	std::string output = "";

	std::string fileName = StringLogic::GlobalToLocalPath(normalizedPath.c_str());
	if (fileName.length() == 0) {
		fileName = normalizedPath;
	}

	if (PHYSFS_exists(fileName.c_str()) == 0)
	{
		FileSystem::Copy(assetsFile, ASSETS_FOLDER, output);
		fileName = output;
	}

	char* buffer = nullptr;
	uint size = FileSystem::LoadToBuffer(fileName.c_str(), &buffer);

	if (buffer != nullptr && size != 0)
	{
		switch (FileSystem::GetTypeFromPath(assetsFile))
		{
		case ImportType::NOTYPE:
		{
			LOG(LogType::L_ERROR, "File extension not supported yet");
			break;
		}
		case ImportType::MESH:
		{
			ModelImporter::Import(normalizedPath.c_str(), buffer, size, app->scene->root);
			break;
		}
		case ImportType::TEXTURE:
		{
			Texture* material = new Texture(normalizedPath.c_str());
			material->LoadToMemory();
			
			app->renderer3D->globalTextures.push_back(material);

			Inspector* inspector = dynamic_cast<Inspector*>(app->editor->GetTab(TabType::INSPECTOR));
			if (inspector && inspector->gameObjectSelected) {
				Material* mat = dynamic_cast<Material*>(inspector->gameObjectSelected->GetComponent(ComponentType::MATERIAL));
				if (mat)
				{
					mat->texture = material;
				}
				else {
					Material* mat = dynamic_cast<Material*>(inspector->gameObjectSelected->AddComponent(ComponentType::MATERIAL));
					mat->texture = material;
				}
			}
			break;
		}

		}
		RELEASE_ARRAY(buffer);
	}
}