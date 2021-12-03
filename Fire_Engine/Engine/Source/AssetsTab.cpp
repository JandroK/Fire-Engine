#include "AssetsTab.h"
#include "FileSystem.h"
#include <algorithm>

AssetsTab::AssetsTab() : Tab()
{
	name = "Assets";
	parentFolder = new Folder(ASSETS_FOLDER, ASSETS_FOLDER);
	parentFolder->parent = nullptr;
	currentFolder = parentFolder;
	currentFolderPath = currentFolder->fullPath;
	//parentFolder->name = ASSETS_FOLDER;
	//parentFolder->fullPath = ASSETS_FOLDER;
	//parentFolder = { ASSETS_FOLDER, nullptr };
}

void AssetsTab::Draw()
{
	if (ImGui::Begin(name.c_str(), &active))
	{
		DrawFolder(currentFolder);
	}
	ImGui::End();
}

void AssetsTab::UpdateAssets()
{
	//LoadAssets(parentFolder);
	Reload();
}

void AssetsTab::LoadAssets(Folder* parentFolder)
{
	std::vector<const char*> files = FileSystem::GetFilesFromFolder(parentFolder->fullPath.c_str());

	for (int i = 0; i < files.size(); i++)
	{
		Asset asset;
		asset.folder = parentFolder->fullPath.c_str();
		asset.name = files.at(i);
		asset.fullPath = asset.folder;
		asset.fullPath += asset.name;
		asset.type = CheckAssetType(asset.name);
		if (asset.type == AssetType::FOLDER)
		{
			Folder* folder = new Folder(asset.name, asset.fullPath + "/");
			folder->parent = parentFolder;
			//folder->name = asset.name;
			//folder->fullPath = asset.fullPath + "/";
			LoadAssets(folder);
			parentFolder->childFolders.push_back(folder);
			std::string assetPath = FileSystem::GetBasePath();
			assetPath = FileSystem::NormalizePath(assetPath.c_str());
			assetPath += folder->fullPath;
			FileSystem::AddPath(assetPath.c_str());
		}
		else if(asset.type != AssetType::OTHER) parentFolder->assets.push_back(asset);
	}

}

AssetType AssetsTab::CheckAssetType(const char* assetName)
{
	std::string ext = assetName;
	int pointPos = ext.find_last_of(".");
	if (pointPos != -1)
	{
		ext = ext.substr(pointPos);

		if (ext == ".fbx" || ext == ".FBX") return AssetType::FBX;

		else if (ext == ".png" || ext == ".PNG" || ext == ".jpg" || ext == ".JPG" || ext == ".dds" || ext == ".DDS") return AssetType::TEXTURE;

		else return AssetType::OTHER;
	}

	else return AssetType::FOLDER;
}

void AssetsTab::DrawFolder(Folder* folder)
{
	ImFont* font = ImGui::GetFont();
	font->Scale = 1.5f;
	ImGui::PushFont(font);
	ImGui::Text(folder->fullPath.c_str());
	font->Scale = 1.0f;
	ImGui::PopFont();

	if (ImGui::Button("Reload"))
		Reload();

	ImGui::Separator();

	if (folder->name != ASSETS_FOLDER)
	{
		if (ImGui::Button("BACK"))
		{
			currentFolder = folder->parent;
			currentFolderPath = currentFolder->fullPath;
		}
	}

	for (int i = 0; i < currentFolder->childFolders.size(); i++)
	{
		if (ImGui::Button(currentFolder->childFolders.at(i)->name))
		{
			currentFolder = currentFolder->childFolders.at(i);
			currentFolderPath = currentFolder->fullPath;
		}
	}

	for (int i = 0; i < currentFolder->assets.size(); i++)
	{
		ImGui::Text(currentFolder->assets.at(i).name);
	}
}

void AssetsTab::Reload()
{
	std::string temp = currentFolder->fullPath;
	currentFolder = parentFolder;
	currentFolderPath = currentFolder->fullPath;
	RemoveSearchPaths(currentFolder);
	RELEASE_VECTOR(parentFolder->childFolders, parentFolder->childFolders.size());
	parentFolder->childFolders.clear();
	parentFolder->assets.clear();
	LoadAssets(currentFolder);
	for (int i = 0; i < currentFolder->childFolders.size(); i++)
	{
		if (currentFolder->childFolders.at(i)->fullPath == temp)
		{
			currentFolder = currentFolder->childFolders.at(i);
			break;
		}
	}
}

void AssetsTab::RemoveSearchPaths(Folder* folder)
{
	for (int i = 0; i < folder->childFolders.size(); i++)
	{
		RemoveSearchPaths(folder->childFolders.at(i));
	}

	std::string assetPath = FileSystem::GetBasePath();
	assetPath = FileSystem::NormalizePath(assetPath.c_str());
	assetPath += folder->fullPath;

	if(folder->parent != nullptr) FileSystem::RemovePath(assetPath.c_str());
}