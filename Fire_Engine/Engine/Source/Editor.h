#pragma once
#include "Module.h"
#include "Globals.h"

#include "imgui/imgui.h"
#include "imgui/imgui_user.h"
#include "imgui/imgui_impl_sdl.h"
#include "imgui/imgui_impl_opengl3.h"

class Tab;
class GameObject;

enum class TabType 
{
	ABOUT,
	CONSOLE,
	CONFIGURATION,
	INSPECTOR,
	HIERARCHY,
	SCENE,
	GAME,
	FOGWAR,
	QUADTREE,
	ASSETS,

	MAX
};

class Editor : public Module
{
public:
	Editor(Application* app, bool start_enabled = true);
	~Editor() {};

	bool Init() override;
	bool Start() override;
	void LogToConsole(const char* msg, LogType _type);
	
	void StartFrame();
	void CheckShortCuts();
	update_status Draw();
	bool DrawWarningTab(std::string text);

	update_status ImGuiMenuBar();
	void NewScene();
	void PrimitiveMenuItem();

	Tab* GetTab(TabType type);
	GameObject* GetGameObjectSelected();
	void SetGameObjectSelected(GameObject* obj);
	void UpdateAssets();

	void AlignWithView();
	void AlignViewWithSelected();
	void ResetViewRotation();
	void ToggleActiveState();
	void Duplicate(GameObject* obj, GameObject* parent);

	bool CleanUp() override;

	void CreateDockSpace();
	void DockSpaceOverViewportCustom(ImGuiViewport* viewport, ImGuiDockNodeFlags dockspaceFlags, ImVec2 position, ImVec2 size, const ImGuiWindowClass* windowClass);

	void DrawShortcut(const char* label, const char* shortcut, bool checkBox = false);

private:
	std::vector<Tab*> tabs;

	bool show_demo_window = false;
	bool warningTab = false;

	std::vector<float> fpsLog;
	std::vector<float> msLog;

	bool updateAssets = false;
};