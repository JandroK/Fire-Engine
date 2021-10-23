#pragma once
#include "Module.h"
#include "Globals.h"
//#include "Resource.h"

class Application;

class ResourceManager:public Module
{
public:
	ResourceManager(Application* app, bool start_enabled = true);
	~ResourceManager();

	bool Init();
	bool Start();
	update_status PreUpdate(float dt);
	update_status Update(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

	int GenerateNewUID();
	//Resource* RequestResource(int uid, const char* libraryPath = nullptr);
};