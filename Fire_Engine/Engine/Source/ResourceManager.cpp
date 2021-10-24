#include "Application.h"
#include "ResourceManager.h"

//Importers
#include "FileSystem.h"

//Resources
#include "Textures.h"
#include "Mesh.h"

ResourceManager::ResourceManager(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	name = "ResourceManager";
}

ResourceManager::~ResourceManager()
{
}

bool ResourceManager::Init()
{
	return true;
}

bool ResourceManager::Start()
{
	return true;
}

bool ResourceManager::CleanUp()
{
	return true;
}
// Generate unique UID
int ResourceManager::GenerateNewUID()
{
	return app->GetNewUID();
}
// Get file type according to its extension
Resource::Type ResourceManager::GetTypeFromLibraryExtension(const char* libraryFile) const
{
	std::string ext(libraryFile);
	ext = ext.substr(ext.find_last_of('.') + 1);

	for (int i = 0; i < ext.length(); i++)
	{
		ext[i] = std::tolower(ext[i]);
	}

	if (ext == "dds")
		return Resource::Type::TEXTURE;
	if (ext == "model")
		return Resource::Type::MODEL;
	if (ext == "mmh")
		return Resource::Type::MESH;
	if (ext == "des")
		return Resource::Type::SCENE;
	if (ext == "shdr")
		return Resource::Type::SHADER;
	if (ext == "mat")
		return Resource::Type::MATERIAL;


	return Resource::Type::UNKNOWN;
}
Resource* ResourceManager::CreateNewResource(const char* assetsFile, uint uid, Resource::Type type)
{
	Resource* ret = nullptr;

	static_assert(static_cast<int>(Resource::Type::UNKNOWN) == 7, "Update all switches with new type");
	switch (type)
	{
	case Resource::Type::TEXTURE: ret = (Resource*) new Texture(uid); break;
	case Resource::Type::MESH: ret = (Resource*) new Mesh(uid); break;
	}

	if (ret != nullptr)
	{
		resources[uid] = ret;
		ret->SetAssetsPath(assetsFile);
		ret->SetLibraryPath(GenLibraryPath(ret->GetUID(), type).c_str());
		ret->IncreaseReferenceCount();
	}

	return ret;
}
// Create new Resource
Resource* ResourceManager::RequestResource(int uid, const char* libraryPath)
{
	//Find if the resource is already loaded
	if (uid <= -1)
		return nullptr;

	std::map<int, Resource*>::iterator it = resources.find(uid);
	if (it != resources.end())
	{
		it->second->IncreaseReferenceCount();
		return it->second;
	}

	//Find the library file (if exists) and load the custom file format
	if (libraryPath != nullptr)
	{
		Resource* ret = nullptr;

		static_assert(static_cast<int>(Resource::Type::UNKNOWN) == 7, "Update all switches with new type");

		//Save check
		if (FileSystem::Exists(libraryPath))
		{
			//uid = 0; //This should be the uid from library
			switch (GetTypeFromLibraryExtension(libraryPath))
			{
			case Resource::Type::TEXTURE: ret = (Resource*) new Texture(uid); break;
			case Resource::Type::MESH: ret = (Resource*) new Mesh(uid); break;
			}

			if (ret != nullptr)
			{
				resources[uid] = ret;
				ret->SetAssetsPath("");
				ret->SetLibraryPath(libraryPath);
				ret->IncreaseReferenceCount();

				ret->LoadToMemory();
			}
			LOG(LogType::L_NORMAL, "Requested resource loaded as new");
		}
		else
			LOG(LogType::L_ERROR, "Requested resource does not exist");

		return ret;
	}

	return nullptr;
}

std::string ResourceManager::GenLibraryPath(uint _uid, Resource::Type _type)
{
	std::string ret = "";
	std::string nameNoExt = std::to_string(_uid);

	switch (_type)
	{
	case Resource::Type::TEXTURE: ret = TEXTURES_FOLDER; ret += nameNoExt; ret += ".dds"; break;
	case Resource::Type::MESH: ret = MESH_FOLDER; ret += nameNoExt; ret += ".mmh"; break;
	}

	return ret;
}
