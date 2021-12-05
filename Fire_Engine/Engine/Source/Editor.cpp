#include "Application.h"
#include "Editor.h"

// Modules
#include "Scene.h"
#include "GameObject.h"
#include "Window.h"
#include "Input.h"
#include "Camera3D.h"
#include "ResourceManager.h"

// Tabs
#include "Tab.h"
#include "Configuration.h"
#include "ConsoleTab.h"
#include "AboutTab.h"
#include "Inspector.h"
#include "Hierarchy.h"
#include "SceneTab.h"
#include "GameTab.h"
#include "FogWarTab.h"
#include "QuadTreeTab.h"
#include "AssetsTab.h"

#include "Primitive.h"
#include "Transform.h"
#include "Material.h"
#include "MeshRenderer.h"
#include "ResourceTexture.h"

#include "Style.h"
#include <string>

Editor::Editor(Application* app, bool start_enabled): Module(app, start_enabled)
{	
	name = "Editor";

	// Define size of the vector
	tabs = std::vector<Tab*>(static_cast<unsigned int>(TabType::MAX), nullptr);

	// Fill the array with the types of tabs 
	// The first must be AboutTab
	tabs[static_cast<unsigned int>(TabType::ABOUT)] = new AboutTab();
	tabs[static_cast<unsigned int>(TabType::CONSOLE)] = new ConsoleTab();
	tabs[static_cast<unsigned int>(TabType::CONFIGURATION)] = new Configuration();
	tabs[static_cast<unsigned int>(TabType::INSPECTOR)] = new Inspector();
	tabs[static_cast<unsigned int>(TabType::HIERARCHY)] = new Hierarchy(app->scene);
	tabs[static_cast<unsigned int>(TabType::SCENE)] = new SceneTab();
	tabs[static_cast<unsigned int>(TabType::GAME)] = new GameTab();
	tabs[static_cast<unsigned int>(TabType::FOGWAR)] = new FogWarTab();
	tabs[static_cast<unsigned int>(TabType::QUADTREE)] = new QuadTreeTab();
	tabs[static_cast<unsigned int>(TabType::ASSETS)] = new AssetsTab();

	currentFolder = &static_cast<AssetsTab*>(GetTab(TabType::ASSETS))->currentFolderPath;

	// Assign a shortcut to each tab
	for (int i = 0; i < tabs.size(); i++)
	{
		tabs[i]->shortcut = i + 1;
	}
	
	stylesList = { "Deep Dark", "Red & Dark", "Green & Blue", "Classic Dark", "Visual Studio", "Gold & Black", "Smooth Dark" };
}

bool Editor::Init()
{
	LOG(LogType::L_NORMAL, "Init editor gui with imgui lib version %s", ImGui::GetVersion());

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->Pos);
	ImGui::SetNextWindowSize(viewport->Size);
	ImGui::SetNextWindowViewport(viewport->ID);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;    // Enable Gamepad Controls

	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows

	
	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();

	// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
	ImGuiStyle& style = ImGui::GetStyle();
	if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	// Setup Platform/Renderer backends
	ImGui_ImplSDL2_InitForOpenGL(App->window->window, App->window->gl_context);
	ImGui_ImplOpenGL3_Init();

	static_cast<AssetsTab*>(GetTab(TabType::ASSETS))->UpdateAssets();

    return true;
}

bool Editor::Start()
{
	Style::SetStyle(style);
    return true;
}

void Editor::LogToConsole(const char* msg, LogType _type)
{
	ConsoleTab* consoleWindow = static_cast<ConsoleTab*>(GetTab(TabType::CONSOLE));

	if (consoleWindow != nullptr)
		consoleWindow->AddLog(msg, _type);
}

void Editor::CreateDockSpace()
{	
	ImGuiViewport* viewport = ImGui::GetMainViewport();

	ImVec2 dockPos(viewport->WorkPos);
	ImGui::SetNextWindowPos(dockPos);

	ImVec2 dockSize(viewport->WorkSize);
	ImGui::SetNextWindowSize(dockSize);

	DockSpaceOverViewportCustom(viewport, ImGuiDockNodeFlags_PassthruCentralNode, dockPos, dockSize, nullptr);
}

void Editor::StartFrame()
{
	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();

	// Check if any key is pressed and disable/enable the tab
	for (int i = 0; i < tabs.size(); i++)
	{
		if (App->input->GetKey(29 + tabs[i]->shortcut) == KEY_UP)
		{
			tabs[i]->active = !tabs[i]->active;
		}
	}

	CheckShortCuts();
}

void Editor::CheckShortCuts()
{
	if (App->input->GetKey(SDL_SCANCODE_LCTRL) == KEY_REPEAT)
	{
		if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT)
		{
			if (App->input->GetKey(SDL_SCANCODE_N) == KEY_UP)
			{
				app->scene->CreateGameObjectEmpty("GameObject");
			}
			else if (App->input->GetKey(SDL_SCANCODE_G) == KEY_UP)
			{
				app->scene->CreateGameObjectParent("GameObjectParent", GetGameObjectSelected());
			}
			else if (App->input->GetKey(SDL_SCANCODE_F) == KEY_UP)
			{
				AlignWithView();
			}
		}
		else if (App->input->GetKey(SDL_SCANCODE_LALT))
		{
			if (App->input->GetKey(SDL_SCANCODE_W) == KEY_UP)
			{
				App->camera->translateSnap = !App->camera->translateSnap;
			}
			else if (App->input->GetKey(SDL_SCANCODE_E) == KEY_UP)
			{
				App->camera->rotateSnap = !App->camera->rotateSnap;
			}
			else if (App->input->GetKey(SDL_SCANCODE_R) == KEY_UP)
			{
				App->camera->scaleSnap = !App->camera->scaleSnap;
			}
		}
		else if (App->input->GetKey(SDL_SCANCODE_N) == KEY_UP)
		{
			warningTab = true;
		}
		else if (App->input->GetKey(SDL_SCANCODE_S) == KEY_UP)
		{
			App->scene->SaveSceneRequest();
		}
		else if (App->input->GetKey(SDL_SCANCODE_D) == KEY_UP)
		{
			Duplicate(GetGameObjectSelected(), GetGameObjectSelected()->GetParent());
		}
	}
	else if (App->input->GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT)
	{
		if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT)
		{
			if (App->input->GetKey(SDL_SCANCODE_N) == KEY_UP)
			{
				app->scene->CreateGameObjectChild("GameObjectChild", GetGameObjectSelected());
			}
			else if (App->input->GetKey(SDL_SCANCODE_F) == KEY_UP)
			{
				AlignViewWithSelected();
			}
			else if (App->input->GetKey(SDL_SCANCODE_A) == KEY_UP)
			{
				ToggleActiveState();
			}
		}
		else if (App->input->GetKey(SDL_SCANCODE_F) == KEY_UP)
		{
			ResetViewRotation();
		}
	}
}

update_status Editor::Draw()
{	
	update_status ret = update_status::UPDATE_CONTINUE;

	// Update assets on assets tab
	if (updateAssets)
	{
		static_cast<AssetsTab*>(GetTab(TabType::ASSETS))->UpdateAssets();
		updateAssets = false;
	}

	StartFrame();
	ret = ImGuiMenuBar();
	CreateDockSpace();

	// Rendering the tabs
	for (unsigned int i = 0; i < tabs.size(); i++)
	{
		if (tabs[i]->active)
		{
			tabs[i]->Draw();
		}
	}
	if (warningTab)
		if (DrawWarningTab("New Scene")) NewScene();
	if (app->input->GetQuit())
		if (DrawWarningTab("Exit Engine")) ret = UPDATE_STOP;
	if (app->resourceManager->GetOverwritting()) app->resourceManager->DrawOverwriteTab();

	ImGui::EndFrame();
	ImGui::Render();
	glViewport(0, 0, (int)ImGui::GetIO().DisplaySize.x, (int)ImGui::GetIO().DisplaySize.y);
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		SDL_Window* backup_current_window = SDL_GL_GetCurrentWindow();
		SDL_GLContext backup_current_context = SDL_GL_GetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		SDL_GL_MakeCurrent(backup_current_window, backup_current_context);
	}

	return ret;
}

bool Editor::DrawWarningTab(std::string text)
{
	ImGui::OpenPopup("Warning");
	bool ret = false;
	if (ImGui::BeginPopupModal("Warning"))
	{
		float offset = ImGui::GetWindowContentRegionMax().x/2 - ImGui::CalcTextSize(text.c_str()).x/2;
		ImGui::SetCursorPosX(offset);
		ImGui::Text(text.c_str());

		ImGui::NewLine();
		ImGui::TextWrapped("Are you sure you want to %s?", text.c_str());
		ImGui::TextWrapped("If you close the scene, your changes will be lost. Do you want continue anyway?");
		ImGui::NewLine();

		offset = ImGui::GetWindowContentRegionMax().x / 2 - ImGui::CalcTextSize("YES").x - 6;
		ImGui::SetCursorPosX(offset);
		if (ImGui::Button("YES"))
		{
			ret = true;
			warningTab = false;
		}
		ImGui::SameLine();
		offset = ImGui::GetWindowContentRegionMax().x / 2 + ImGui::CalcTextSize("NO").x - 6;
		ImGui::SetCursorPosX(offset);
		if (ImGui::Button("NO"))
		{
			warningTab = false;
			app->input->SetQuit(false);
		}
	}
	ImGui::EndPopup();
	return ret;
}

update_status Editor::ImGuiMenuBar()
{
	// Create a MenuBar
	update_status ret = update_status::UPDATE_CONTINUE;

	if (ImGui::BeginMainMenuBar())
	{
		bool isSelected = true;
		if (GetGameObjectSelected() == nullptr) isSelected = false;
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Quit", "ESC"))
				ret = UPDATE_STOP;

			if (ImGui::MenuItem("New Scene","Ctrl+N"))
				warningTab = true;

			if (ImGui::MenuItem("Save Scene", "Ctrl+S"))
				app->scene->SaveSceneRequest();

			if (ImGui::MenuItem("Load Scene"))
				app->scene->LoadSceneRequest();

			ImGui::Separator();
			if (ImGui::MenuItem("Import Asset"))
			{
				std::string path = app->resourceManager->OpenFileName();
				if (!path.empty())
				{
					path = path.substr(3); // Path has "C:\\" which makes FileSystem think it's a dangerous file and won't load it.
					app->resourceManager->ImportFile(path.c_str());
				}
			}

			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Edit"))
		{
			if (!isSelected)
			{
				ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(128, 128, 128, 255));
				ImGui::PushStyleColor(ImGuiCol_HeaderHovered, IM_COL32(128, 128, 128, 100));
			}
			if (ImGui::MenuItem("Duplicate", "Ctrl+D"))
				if (isSelected) Duplicate(GetGameObjectSelected(), GetGameObjectSelected()->GetParent());

			if (!isSelected)
			{
				ImGui::PopStyleColor();
				ImGui::PopStyleColor();
			}

			ImGui::Separator();

			//Translate snap options
			DrawShortcut("Snap Translate", "Ctrl+Alt+W", true);
			ImGui::Checkbox("Snap Translate", &app->camera->translateSnap);
			if (!app->camera->translateSnap)
			{
				ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(128, 128, 128, 255));
				ImGui::PushStyleColor(ImGuiCol_HeaderHovered, IM_COL32(128, 128, 128, 100));
			}
			ImGui::PushID("Translate");
			if (ImGui::BeginMenu("Snap Options"))
			{
				if (ImGui::SliderFloat("X", &app->camera->tSnap[0], 0.0f, 10.0f, "%.2f"));
				if (ImGui::SliderFloat("Y", &app->camera->tSnap[1], 0.0f, 10.0f, "%.2f"));
				if (ImGui::SliderFloat("Z", &app->camera->tSnap[2], 0.0f, 10.0f, "%.2f"));
				if (ImGui::SliderFloat("All", &app->camera->allTsnap, 0.0f, 10.0f, "%.2f"))
				{
					app->camera->tSnap[0] = app->camera->allTsnap;
					app->camera->tSnap[1] = app->camera->allTsnap;
					app->camera->tSnap[2] = app->camera->allTsnap;
				}
				ImGui::EndMenu();
			}
			ImGui::PopID();
			if (!app->camera->translateSnap)
			{
				ImGui::PopStyleColor();
				ImGui::PopStyleColor();
			}

			//Rotation snap options
			DrawShortcut("Snap Rotation", "Ctrl+Alt+E", true);
			ImGui::Checkbox("Snap Rotation", &app->camera->rotateSnap);
			if (!app->camera->rotateSnap)
			{
				ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(128, 128, 128, 255));
				ImGui::PushStyleColor(ImGuiCol_HeaderHovered, IM_COL32(128, 128, 128, 100));
			}
			ImGui::PushID("Rotation");
			if (ImGui::BeginMenu("Snap Options"))
			{
				ImGui::SliderInt("All", &app->camera->allRsnap, 0, 90);
				ImGui::EndMenu();
			}
			ImGui::PopID();
			if (!app->camera->rotateSnap)
			{
				ImGui::PopStyleColor();
				ImGui::PopStyleColor();
			}

			//Sacle snap options
			DrawShortcut("Snap Scale", "Ctrl+Alt+R", true);
			ImGui::Checkbox("Snap Scale", &app->camera->scaleSnap);
			if (!app->camera->scaleSnap)
			{
				ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(128, 128, 128, 255));
				ImGui::PushStyleColor(ImGuiCol_HeaderHovered, IM_COL32(128, 128, 128, 100));
			}
			ImGui::PushID("Scale");
			if (ImGui::BeginMenu("Snap Options"))
			{
				ImGui::SliderFloat("All", &app->camera->allSsnap, 0.0f, 1.0f, "%.2f");
				ImGui::EndMenu();
			}
			ImGui::PopID();
			if (!app->camera->scaleSnap)
			{
				ImGui::PopStyleColor();
				ImGui::PopStyleColor();
			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("View"))
		{
			for (int i = 0; i < tabs.size(); i++)
			{
				// Name  ShortCut  State
				if (ImGui::MenuItem(tabs[i]->name.c_str(), std::to_string(tabs[i]->shortcut).c_str(), tabs[i]->active, &tabs[i]->active))
					tabs[i]->active = !tabs[i]->active;
			}
			ImGui::EndMenu();
		}
		// Menu of game objects
		if (ImGui::BeginMenu("Game Objects"))
		{
			if (ImGui::MenuItem("Create Empty", "Ctrl+Shift+N"))
			{
				app->scene->CreateGameObjectEmpty("GameObject");
			}
			if (ImGui::MenuItem("Create Child", "Alt+Shift+N"))
			{
				app->scene->CreateGameObjectChild("GameObjectChild", GetGameObjectSelected());
			}
			if (ImGui::MenuItem("Create Parent", "Ctrl+Shift+G"))
			{
				app->scene->CreateGameObjectParent("GameObjectParent", GetGameObjectSelected());
			}
			if (ImGui::BeginMenu("3D Object"))
			{
				PrimitiveMenuItem();
				ImGui::EndMenu();
			}
			if (ImGui::MenuItem("Camera"))
			{
				app->scene->CreateCamera();
			}
			ImGui::Separator();
			if (ImGui::MenuItem("Reset view rotation", "Alt+F"))
			{
				ResetViewRotation();
			}
			if (!isSelected)
			{
				ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(128, 128, 128, 255));
				ImGui::PushStyleColor(ImGuiCol_HeaderHovered, IM_COL32(128, 128, 128, 100));
			}
			if (ImGui::MenuItem("Align with view", "Ctrl+Shift+F"))
			{
				if (isSelected) AlignWithView();
			}
			if (ImGui::MenuItem("Align view to selected", "Alt+Shift+F"))
			{
				if (isSelected) AlignViewWithSelected();
			}
			if (ImGui::MenuItem("Toggle Active State", "Alt+Shift+A"))
			{
				if (isSelected) ToggleActiveState();
			}
			if (!isSelected)
			{
				ImGui::PopStyleColor();
				ImGui::PopStyleColor();
			}

			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Help"))
		{
			if (ImGui::MenuItem("Documentation"))
			{
				ShellExecute(0, 0, "https://github.com/JandroK/Fire-Engine/wiki", 0, 0, SW_SHOW);
			}
			if (ImGui::MenuItem("Download latest"))
			{
				ShellExecute(0, 0, "https://github.com/JandroK/Fire-Engine/releases", 0, 0, SW_SHOW);
			}
			if (ImGui::MenuItem("Report a bug"))
			{
				ShellExecute(0, 0, "https://github.com/JandroK/Fire-Engine/issues", 0, 0, SW_SHOW);
			}
			if (ImGui::MenuItem("About"))
			{
				tabs[0]->active = !tabs[0]->active;
			}			
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Set Style"))
		{
			for (int i = 0; i < stylesList.size(); i++)
			{
				if (ImGui::MenuItem(stylesList.at(i).c_str()))
				{
					Style::SetStyle(i);
					style = i;
				}
			}
			ImGui::Separator();

			float auxAlpha = alphaStyle;
			ImGui::Text("PopUp Alpha:");
			ImGui::PushItemWidth(100);
			if (ImGui::InputFloat("##Alpha", &alphaStyle, 0.1f))
			{
				if (alphaStyle < auxAlpha)
					Style::SetAlpha(0.9);
				else Style::SetAlpha(1.1);
				alphaStyle = auxAlpha;
			}
			ImGui::PopItemWidth();

			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}
	return ret;
}

void Editor::NewScene()
{
	SetGameObjectSelected(nullptr);
	app->scene->CleanUp(); //Clean GameObjects 
	app->scene->Init();
	app->camera->ReStartCamera();
}

void Editor::PrimitiveMenuItem()
{
	if (ImGui::MenuItem("Cube", nullptr))
	{
		PrimitiveCube cubePrim = PrimitiveCube();
		cubePrim.InnerMesh();
		cubePrim.mesh->LoadToMemory();
		cubePrim.mesh->GenerateBounds();

		app->scene->CreatePrimitive("Cube", cubePrim.mesh);
	}
	if (ImGui::MenuItem("Sphere", nullptr))
	{
		PrimitiveSphere spherePrim = PrimitiveSphere();
		spherePrim.InnerMesh();
		spherePrim.mesh->LoadToMemory();
		spherePrim.mesh->GenerateBounds();

		app->scene->CreatePrimitive("Sphere", spherePrim.mesh);
	}
	if (ImGui::MenuItem("Cylinder", nullptr))
	{
		PrimitiveCylinder cylinderPrim = PrimitiveCylinder();
		cylinderPrim.InnerMesh();
		cylinderPrim.mesh->LoadToMemory();
		cylinderPrim.mesh->GenerateBounds();

		app->scene->CreatePrimitive("Cylinder", cylinderPrim.mesh);
	}
	if (ImGui::MenuItem("Pyramid", nullptr))
	{
		PrimitivePyramid pyramidPrim = PrimitivePyramid();
		pyramidPrim.InnerMesh();
		pyramidPrim.mesh->LoadToMemory();
		pyramidPrim.mesh->GenerateBounds();

		app->scene->CreatePrimitive("Pyramid", pyramidPrim.mesh);
	}
}

bool Editor::CleanUp()
{
	bool ret= true;
	LOG(LogType::L_NORMAL, "Editor CleanUp");

	// CleanUp all tabs
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	RELEASE_VECTOR(tabs, tabs.size());
	tabs.clear();

	fpsLog.clear();
	msLog.clear();

	return ret;
}

Tab* Editor::GetTab(TabType type)
{
	unsigned int vecPosition = static_cast<unsigned int>(type);

	return (vecPosition < tabs.size()) ? tabs[vecPosition] : tabs[vecPosition];
}

GameObject* Editor::GetGameObjectSelected()
{
	return static_cast<Inspector*>(GetTab(TabType::INSPECTOR))->gameObjectSelected;
}
void Editor::SetStyle(int _style)
{
	style = _style;
	Style::SetStyle(style);
}
void Editor::SetGameObjectSelected(GameObject* obj)
{
	static_cast<Inspector*>(app->editor->GetTab(TabType::INSPECTOR))->gameObjectSelected = obj;
}

void Editor::DockSpaceOverViewportCustom(ImGuiViewport* viewport, ImGuiDockNodeFlags dockspaceFlags, ImVec2 position, ImVec2 size, const ImGuiWindowClass* windowClass)
{
	ImGuiWindowFlags host_window_flags = 0;
	host_window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDocking;
	host_window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;// | ImGuiWindowFlags_MenuBar;

	if (dockspaceFlags & ImGuiDockNodeFlags_PassthruCentralNode)
		host_window_flags |= ImGuiWindowFlags_NoBackground;

	char label[32];
	ImFormatString(label, IM_ARRAYSIZE(label), "DockSpaceViewport_%08X", viewport->ID);

	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 10.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.5f, 0.5f));

	ImGui::Begin(label, NULL, host_window_flags);

	ImGui::PopStyleVar(3);
	ImGuiID dockspaceId = ImGui::GetID("DockSpace");
	ImGui::DockSpace(dockspaceId, ImVec2(0.0f, 0.0f), dockspaceFlags, windowClass);
	ImGui::End();
}

void Editor::AlignWithView()
{
	GameObject* temp = GetGameObjectSelected();
	if (temp != nullptr)
	{
		Transform* transform = static_cast<Transform*>(temp->GetComponent(ComponentType::TRANSFORM));
		float4x4 matrix = transform->GetGlobalTransform();
		matrix.SetTranslatePart(app->camera->position);
		float3x3 rot{ app->camera->right, app->camera->up, app->camera->front };
		matrix.SetRotatePart(rot.ToQuat());
		transform->SetTransformMFromM(matrix);
	}
}

void Editor::AlignViewWithSelected()
{
	GameObject* temp = GetGameObjectSelected();
	if (temp != nullptr)
	{
		Transform* transform = static_cast<Transform*>(temp->GetComponent(ComponentType::TRANSFORM));
		float4x4 matrix = transform->GetGlobalTransform();

		app->camera->position = transform->GetPosition();
		float3x3 rot = matrix.RotatePart();
		app->camera->right = rot.Col3(0);
		app->camera->front = rot.Col3(1);
		app->camera->up = rot.Col3(2);
		app->camera->CalculateViewMatrix();
	}
}

void Editor::ResetViewRotation()
{
		app->camera->right = app->camera->oldRotation.Col3(0);
		app->camera->front = app->camera->oldRotation.Col3(1);
		app->camera->up = app->camera->oldRotation.Col3(2);
		app->camera->CalculateViewMatrix();
}

void Editor::ToggleActiveState()
{
	GameObject* temp = GetGameObjectSelected();
	if (temp != nullptr)
		temp->active = !temp->active;
}

void Editor::Duplicate(GameObject* obj, GameObject* parent)
{
		GameObject* duplicated = new GameObject(obj->name.c_str());
		static_cast<Transform*>(duplicated->GetCompoments().at(0))->operator=(static_cast<Transform*>(obj->GetCompoments().at(0)));

		for (int i = 1; i < obj->GetCompoments().size(); i++)
		{
			duplicated->AddComponent(obj->GetCompoments().at(i)->GetType());
			switch (obj->GetCompoments().at(i)->GetType())
			{
			case ComponentType::MESHRENDERER:
				static_cast<MeshRenderer*>(duplicated->GetCompoments().at(i))->SetMesh(static_cast<MeshRenderer*>(obj->GetCompoments().at(i))->GetMesh());
				static_cast<MeshRenderer*>(duplicated->GetCompoments().at(i))->globalAABB = static_cast<MeshRenderer*>(obj->GetCompoments().at(i))->globalAABB;
				static_cast<MeshRenderer*>(duplicated->GetCompoments().at(i))->globalOBB = static_cast<MeshRenderer*>(obj->GetCompoments().at(i))->globalOBB;
				break;

			case ComponentType::MATERIAL:
				//static_cast<Material*>(duplicated->GetCompoments().at(i))->texture = new Texture(static_cast<Material*>(obj->GetCompoments().at(i))->texture->GetAssetPath());
				//static_cast<Material*>(duplicated->GetCompoments().at(i))->texture->texHeight = static_cast<Material*>(obj->GetCompoments().at(i))->texture->texHeight;
				//static_cast<Material*>(duplicated->GetCompoments().at(i))->texture->texWidth = static_cast<Material*>(obj->GetCompoments().at(i))->texture->texWidth;
				//static_cast<Material*>(duplicated->GetCompoments().at(i))->texture->textureID = static_cast<Material*>(obj->GetCompoments().at(i))->texture->textureID;
				static_cast<Material*>(duplicated->GetCompoments().at(i))->texture = static_cast<Material*>(obj->GetCompoments().at(i))->texture;
				break;
			default:
				break;
			}
			//duplicated->GetCompoments().at(i) = temp->GetCompoments().at(i);
			duplicated->GetCompoments().at(i)->SetOwner(duplicated);
		}
		duplicated->SetParent(parent);
		duplicated->GetParent()->AddChildren(duplicated);

		for (int i = 0; i < obj->GetChildrens().size(); i++)
		{
			Duplicate(obj->GetChildrens().at(i), duplicated);
		}
		SetGameObjectSelected(duplicated);
}

void Editor::DrawShortcut(const char* label, const char* shortcut, bool ckeckbox)
{
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	ImVec2 pos = window->DC.CursorPos;
	if (ckeckbox)
	{
		pos.x += 23;
		pos.y += 3;
	}
	const ImGuiMenuColumns* offsets = &window->DC.MenuColumns;
	ImGuiContext& g = *GImGui;
	ImGuiStyle& style = g.Style;
	ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);
	float shortcut_w = (shortcut && shortcut[0]) ? ImGui::CalcTextSize(shortcut, NULL).x : 0.0f;
	float checkmark_w = IM_FLOOR(g.FontSize * 1.20f);
	float min_w = window->DC.MenuColumns.DeclColumns(NULL, label_size.x, shortcut_w, checkmark_w); // Feedback for next frame
	float stretch_w = ImMax(0.0f, ImGui::GetContentRegionAvail().x - min_w);
	ImGui::PushStyleColor(ImGuiCol_Text, style.Colors[ImGuiCol_TextDisabled]);
	ImGui::RenderText(pos + ImVec2(offsets->OffsetShortcut + stretch_w, 0.0f), shortcut, NULL, false);
	ImGui::PopStyleColor();
}

void Editor::UpdateAssets()
{
	updateAssets = true;
}

std::string* Editor::GetCurrentFolder()
{
	return currentFolder;
}

Asset* Editor::GetSelectedAsset()
{
	return static_cast<AssetsTab*>(GetTab(TabType::ASSETS))->GetSelectedAsset();
}

void Editor::DestroySelectedAsset()
{
	static_cast<AssetsTab*>(GetTab(TabType::ASSETS))->GetSelectedAsset()->Destroy();
}