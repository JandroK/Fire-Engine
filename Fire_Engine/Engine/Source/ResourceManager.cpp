#include "Application.h"
#include "ResourceManager.h"

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

update_status ResourceManager::PreUpdate(float dt)
{
	return UPDATE_CONTINUE;
}

update_status ResourceManager::Update(float dt)
{
	return UPDATE_CONTINUE;
}

update_status ResourceManager::PostUpdate(float dt)
{
	return UPDATE_CONTINUE;
}

bool ResourceManager::CleanUp()
{
	return true;
}

int ResourceManager::GenerateNewUID()
{
	return 0;
}

//Resource* ResourceManager::RequestResource(int uid, const char* libraryPath)
//{
//	return nullptr;
//}
