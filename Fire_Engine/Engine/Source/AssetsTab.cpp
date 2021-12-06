#include "AssetsTab.h"
#include "FileSystem.h"

#include "Application.h"
#include "ResourceManager.h"

#include <algorithm>
#include "IconsFontAwesome5.h"

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
		else if (asset.type != AssetType::OTHER)
		{
			asset.libraryPath = app->resourceManager->LibraryFromAssets(asset.fullPath.c_str());
			parentFolder->assets.push_back(asset);
		}
	}

}

AssetType AssetsTab::CheckAssetType(const char* assetName)
{
	std::string ext = assetName;
	int pointPos = ext.find_last_of(".");
	if (pointPos != -1)
	{

		for (int i = 0; i < ext.length(); i++)
		{
			ext[i] = std::tolower(ext[i]);
		}

		ext = ext.substr(pointPos);

		if (ext == ".fbx") return AssetType::FBX;

		else if (ext == ".png" || ext == ".jpg" || ext == ".dds" || ext == ".tga") return AssetType::TEXTURE;

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

	if (ImGui::Button(ICON_FA_CIRCLE_NOTCH" Reload"))
		Reload();

	ImGui::Separator();

	if (folder->name != ASSETS_FOLDER)
	{
		if (ImGui::Button(ICON_FA_ARROW_LEFT))
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

	ImGuiTreeNodeFlags nodeFlags;

	for (int i = 0; i < currentFolder->assets.size(); i++)
	{
		if (currentFolder->assets.at(i).remove)
		{
			currentFolder->assets.erase(std::next(currentFolder->assets.begin(), i));
			break;
		}
		if (selectedAsset == &currentFolder->assets.at(i)) nodeFlags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_Selected;
		else nodeFlags = ImGuiTreeNodeFlags_Leaf;
		if (ImGui::TreeNodeEx(&currentFolder->assets.at(i), nodeFlags, currentFolder->assets.at(i).name))
		{
			if (ImGui::IsItemClicked())
			{
				selectedAsset = &currentFolder->assets.at(i);
			}

			ImGui::TreePop();
		}
		//ImGui::Text(currentFolder->assets.at(i).name);
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
	ReloadRecursiveFolder(currentFolder, temp.c_str());
}
void AssetsTab::ReloadRecursiveFolder(Folder* folder, const char* goal)
{
	for (int i = 0; i < folder->childFolders.size(); i++)
	{
		if (!folder->childFolders.at(i)->childFolders.empty()) ReloadRecursiveFolder(folder->childFolders.at(i), goal);
		if (folder->childFolders.at(i)->fullPath == goal)
		{
			currentFolder = folder->childFolders.at(i);
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

Asset* AssetsTab::GetSelectedAsset()
{
	return selectedAsset;
}

void Asset::Destroy()
{
	FileSystem::Remove(this->fullPath.c_str());
	
	if(app->resourceManager->CheckModel(this->fullPath.c_str()))
	{
		app->resourceManager->DestroyModel(this->libraryPath.c_str());
	}
	
	FileSystem::Remove(this->libraryPath.c_str());

	//TODO: remove library file with GUID on .meta file
	/*std::string libraryPath;
	FileSystem::Remove(libraryPath.c_str());*/

	//TODO: remove .meta file
	this->remove = true;
}