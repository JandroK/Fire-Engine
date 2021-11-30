#pragma once
#include "Module.h"
#include "Globals.h"
#include "Resource.h"

#include<map>

class Application;
class Texture;

class ResourceManager :public Module
{
public:
	ResourceManager(Application* app, bool start_enabled = true);
	~ResourceManager() {};

	bool Init()override;
	bool Start()override;
	bool CleanUp()override;

	void ImportFile(const char* assetsFile);
	bool GetOverwritting() { return overwritting; };
	void Overwrite();
	void DrawOverwriteTab();
	void NewCounterFile();

	std::string OpenFileName(char* filter = "All Files (*.*)\0*.*\0""Textues (*.png, .dds*)\0*.PNG*;*.DDS*\0""3D models (*.fbx*)\0*.fbx*\0", HWND owner = NULL);

	Texture* logo;
	Texture* defaultTexture;
	Texture* backButton;
	Texture* addButton;

private:
	// Overriding file variables
	Resource* ovResource;
	bool overwritting = false;
	const char* ovPath;
	char* ovBuffer;
	uint ovSize;
};
