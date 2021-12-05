#include "Application.h"
#include "ResourceManager.h"

// Modules
#include "Scene.h"
#include "Editor.h"
#include "Camera3D.h"

//Importers
#include "FileSystem.h"
#include "TextureLoader.h"
#include "MeshLoader.h"
#include "ModelImporter.h"


//Resources
#include "ResourceTexture.h"
#include "ResourceMesh.h"

//Tabs
#include "Inspector.h" 

//Components
#include "Material.h"

ResourceManager::ResourceManager(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	name = "ResourceManager";	
}

bool ResourceManager::Init()
{	
	FileSystem::FSInit();	

	return true;
}

bool ResourceManager::Start()
{
	// Import Icons
	logo = new Texture("Resources/logo.png", "logo");
	logo->LoadToMemory();


	backButton = new Texture("Resources/icon_back.png", "backButton");
	backButton->LoadToMemory();

	addButton = new Texture("Resources/icon_add.png", "addButton");
	addButton->LoadToMemory();

	playButton = new Texture("Resources/icon_play.png", "addButton");
	playButton->LoadToMemory();

	pauseButton = new Texture("Resources/icon_pause.png", "addButton");
	pauseButton->LoadToMemory();

	stepButton = new Texture("Resources/icon_step.png", "addButton");
	stepButton->LoadToMemory();

	stopButton = new Texture("Resources/icon_stop.png", "addButton");
	stopButton->LoadToMemory();

	defaultTexture = new Texture("Materials/default_texture.dds", "defaultTexture");
	defaultTexture->LoadToMemory();
	defaultTexture->defaultT = true;
	defaultTexture->SetLibraryPath("Materials/default_texture.dds");

	whiteTexture = new Texture("Materials/white_texture.dds", "whiteTexture");
	whiteTexture->LoadToMemory();
	whiteTexture->defaultT = true;
	whiteTexture->SetLibraryPath("Materials/white_texture.dds");

	greenTexture = new Texture("Materials/green_texture.dds", "greenTexture");
	greenTexture->LoadToMemory();
	greenTexture->defaultT = true;
	greenTexture->SetLibraryPath("Materials/green_texture.dds");

	currentFolderPath = app->editor->GetCurrentFolder();

	return true;
}

bool ResourceManager::CleanUp()
{
	RELEASE(logo);
	RELEASE(backButton);
	RELEASE(addButton);
	RELEASE(defaultTexture);
	RELEASE(whiteTexture);
	RELEASE(greenTexture);

	FileSystem::FSDeInit();
	return true;
}

void ResourceManager::ImportFile(const char* assetsFile)
{
	// Assets tab stuff

	std::string normalizedPath = FileSystem::NormalizePath(assetsFile);
	bool exists = false;
	exists = FileSystem::Exists(assetsFile);
	// If file was grad and dropped from somewhere else than current folder, copy it to current folder
	if (dragDropped)
	{
		std::string temp;
		FileSystem::GetFileName(normalizedPath.c_str(), temp, true);
		std::string fileName = *currentFolderPath;
		fileName += temp;
		exists = FileSystem::Exists(fileName.c_str());
		if (!exists)
		{
			std::string dst = fileName;
			char* buffer = nullptr;
			FileSystem::Copy(normalizedPath.c_str(), dst.c_str(), buffer);
			RELEASE_ARRAY(buffer);
		}
		dragDropped = false;
	}

	std::string output = "";

	char* buffer = nullptr;
	uint size = FileSystem::LoadToBuffer(normalizedPath.c_str(), &buffer);

	if (buffer != nullptr && size != 0)
	{
		switch (FileSystem::GetTypeFromPath(assetsFile))
		{
		case ImportType::NOTYPE:
		{
			LOG(LogType::L_ERROR, "File extension not supported yet");
			break;
		}
		case ImportType::MESH:
		{
			if(!exists) app->editor->UpdateAssets();
			ModelImporter::Import(normalizedPath.c_str(), buffer, size, app->scene->root);
			break;
		}
		case ImportType::TEXTURE:
		{
			Texture* material = new Texture(normalizedPath.c_str());

			if (FileSystem::Exists(material->GetLibraryPath()))
			{
				overwritting = true;
				ovResource = material;
				ovBuffer = buffer;
				ovPath = material->GetLibraryPath();
			}
			else
			{
				if(!exists) app->editor->UpdateAssets();
				material->Import(buffer, size, material->GetLibraryPath());
				material->LoadToMemory();

				GameObject* objSelected = app->editor->GetGameObjectSelected();
				if (objSelected != nullptr) {
					Material* mat = static_cast<Material*>(objSelected->GetComponent(ComponentType::MATERIAL));
					if (mat) mat->texture = material;
					else
					{
						Material* mat = static_cast<Material*>(objSelected->AddComponent(ComponentType::MATERIAL));
						mat->texture = material;
					}
				}
				break;
			}
		}
		default:
			break;
		}
		if (!overwritting) RELEASE_ARRAY(buffer); // Having this inside an if may caouse memory leaks;
		}
}

void ResourceManager::Overwrite()
{
	app->editor->UpdateAssets();
	switch (ovResource->GetType())
	{
	case ResourceType::MESH:
	{
		ModelImporter::Import(ovPath, ovBuffer, ovSize, app->scene->root);
		break;
	}
	case ResourceType::TEXTURE:
	{
		Texture* material = (Texture*)ovResource;
		material->Import(ovBuffer, ovSize, ovPath);

		ovResource->LoadToMemory();

		Inspector* inspector = static_cast<Inspector*>(app->editor->GetTab(TabType::INSPECTOR));
		if (inspector && inspector->gameObjectSelected) {
			Material* mat = static_cast<Material*>(inspector->gameObjectSelected->GetComponent(ComponentType::MATERIAL));
			if (mat) mat->texture = material;
			else
			{
				Material* mat = static_cast<Material*>(inspector->gameObjectSelected->AddComponent(ComponentType::MATERIAL));
				mat->texture = material;
			}
		}
		break;
	}
	default:
		break;
	}

	overwritting = false;
	RELEASE_ARRAY(ovBuffer);
}

void ResourceManager::DrawOverwriteTab()
{
	if (ImGui::Begin("Warning"))
	{
		float offset = ImGui::GetWindowContentRegionMax().x / 2 - ImGui::CalcTextSize("Override file").x / 2;
		ImGui::SetCursorPosX(offset);
		ImGui::Text("Override file");

		ImGui::NewLine();
		ImGui::TextWrapped("Are you sure you want to overwrite %s?", ovPath);
		ImGui::TextWrapped("You can create a new file with a counter on it's name.");
		ImGui::NewLine();

		offset = ImGui::GetWindowContentRegionMax().x / 3 - ImGui::CalcTextSize("OVERWRITE").x - 6;
		ImGui::SetCursorPosX(offset);
		if (ImGui::Button("OVERWRITE"))
		{
			Overwrite();
			overwritting = false;
		}
		ImGui::SameLine();
		offset = ImGui::GetWindowContentRegionMax().x / 3 + ImGui::CalcTextSize("CANCEL").x / 2;
		ImGui::SetCursorPosX(offset);
		if (ImGui::Button("CREATE NEW"))
		{
			NewCounterFile();
			overwritting = false;
		}
		ImGui::SameLine();
		offset = ImGui::GetWindowContentRegionMax().x / 3 * 2 + ImGui::CalcTextSize("CANCEL").x - 6;
		ImGui::SetCursorPosX(offset);
		if (ImGui::Button("CANCEL"))
		{
			overwritting = false;
		}
	}
	ImGui::End();
}

void ResourceManager::NewCounterFile()
{
	std::string num = "_";
	std::string path(ovPath);
	std::string ext = path.substr(path.find_last_of("."));
	path = path.substr(0, path.find_last_of(".")).c_str();
	std::string resetter = path.c_str();

	std::string tester = path;
	for (int i = 1; !num.compare("_"); i++)
	{
		path = path + "_" + std::to_string(i) + ext;
		if (!FileSystem::Exists(path.c_str()))
		{
			ovPath = path.c_str();
			break;
		}
		path = resetter;
	}

	Overwrite();
}

// Returns an empty string if dialog is canceled
std::string ResourceManager::OpenFileName(char* filter, HWND owner) {
	OPENFILENAME ofn;
	char fileName[MAX_PATH] = "";
	ZeroMemory(&ofn, sizeof(ofn)); // Limpia el bloque de memoria de la variable "ofn"

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = owner; // Ventana que tiene la dialogue box
	ofn.lpstrFilter = filter; // Filtros para los tipos de archivo
	ofn.lpstrFile = fileName; // Guarda el path al archivo seleccionado
	ofn.nMaxFile = MAX_PATH; // Tamaño máximo del buffer de "ofn.lpstrFile"
	ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY; // Flags para el dialogue box
	// OFN_EXPLORER: las modificaciones de Open dialogue box usan métodos custom con el estilo del explorer.
	// OFN_FILEMUSTEXIST: en el text field del doalogue box solo se pueden escribir nombres de archivos que existan.
	// OFN_HIDEREADONLY: oculta la checkBox de "solo lectura";
	ofn.lpstrDefExt = ""; // Si el usuario no escribe la extension del archivo, se usa la que tiene este por defecto.

	std::string fileNameStr;

	if (GetOpenFileName(&ofn)) // Abre el dialogue box
		fileNameStr = fileName;

	return fileNameStr;
}