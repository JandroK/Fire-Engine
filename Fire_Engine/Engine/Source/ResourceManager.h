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

	std::string LibraryFromAssets(const char* assetsPath);
	ResourceType GetTypeFromExt(std::string ext);

	// Custom model format
	bool CheckModel(const char* assetsPath);
	void DestroyModel(const char* libraryPath);

	std::string OpenFileName(char* filter = "All Files (*.*)\0*.*\0""Textues (*.png, .dds, .jpg, .tga*)\0*.PNG*;*.DDS*;*.JPG*;*.TGA*\0""3D models (*.fbx*)\0*.fbx*\0", HWND owner = NULL);

	Texture* logo;

	Texture* defaultTexture;
	Texture* whiteTexture;
	Texture* greenTexture;

	bool dragDropped = false;

private:
	// Overriding file variables
	Resource* ovResource;
	bool overwritting = false;
	const char* ovPath;
	char* ovBuffer;
	uint ovSize;
	std::string* currentFolderPath;
};
