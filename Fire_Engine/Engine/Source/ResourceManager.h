#pragma once
#include "Module.h"
//#include "Resource.h"
#include "Globals.h"
#include "Application.h"

//class Application;

class ResourceManager : public Module
{
public:
	ResourceManager(Application* app, bool start_enabled = true);
	virtual ~ResourceManager();

	bool Init();
	bool Start();
	update_status PreUpdate(float dt);
	bool CleanUp();

	int GenerateNewUID();
	//Resource* RequestResource(int uid, const char* libraryPath = nullptr);
};

