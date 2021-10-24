#pragma once
#include "Module.h"
#include "Globals.h"
#include "Resource.h"

#include<map>

class Application;

class ResourceManager :public Module
{
public:
	ResourceManager(Application* app, bool start_enabled = true);
	~ResourceManager();

	bool Init();
	bool Start();
	bool CleanUp();

	int GenerateNewUID();
	Resource::Type GetTypeFromLibraryExtension(const char* libraryFile) const;

	Resource* CreateNewResource(const char* assetsFile, uint uid, Resource::Type type);
	Resource* RequestResource(int uid, const char* libraryPath = nullptr);

	std::string GenLibraryPath(uint _uid, Resource::Type _type);

private:
	std::map<int, Resource*> resources;
};
