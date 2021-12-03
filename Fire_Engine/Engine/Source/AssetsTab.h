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
	void Destroy();

	AssetType type;
	const char* name;
	const char* folder;
	std::string fullPath;
	bool remove = false;
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
	void Reload();

	void RemoveSearchPaths(Folder* folder);

	Asset* GetSelectedAsset();

	std::string currentFolderPath;

private:
	Folder* parentFolder;
	Folder* currentFolder;
	void LoadAssets(Folder* folder);
	AssetType CheckAssetType(const char* assetName);
	Asset* selectedAsset = nullptr;
};