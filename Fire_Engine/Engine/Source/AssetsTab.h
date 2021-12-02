#pragma once
#include "Tab.h"
#include <vector>
#include "Globals.h"

enum class AssetType
{
	TEXTURE,
	FBX,
	FOLDER,
	OTHER
};

class Asset
{
public:
	AssetType type;
	const char* name;
	const char* folder;
	std::string fullPath;
};

class Folder
{
public:
	Folder(const char* name, std::string fullpath) : name(name), fullPath(fullpath) {};
	Folder() {};
	~Folder() {};
	const char* name = "";
	std::string fullPath;
	std::vector<Asset> assets;
	std::vector<Folder*> childFolders;
	Folder* parent;
};

class AssetsTab : public Tab
{
public:
	AssetsTab();
	virtual ~AssetsTab() { RELEASE_VECTOR(parentFolder->childFolders, parentFolder->childFolders.size()); RELEASE(parentFolder)};

	void Draw() override;
	void DrawFolder(Folder* folder);

	void UpdateAssets();

private:
	Folder* parentFolder;
	Folder* currentFolder;
	void LoadAssets(Folder* folder);
	AssetType CheckAssetType(const char* assetName);
};