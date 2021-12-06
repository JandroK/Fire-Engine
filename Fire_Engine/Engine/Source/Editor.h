#pragma once
#include "Module.h"
#include "Globals.h"

#include "imgui/imgui.h"
#include "imgui/imgui_user.h"
#include "imgui/imgui_impl_sdl.h"
#include "imgui/imgui_impl_opengl3.h"

class Tab;
class GameObject;
class Asset;

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
	void TopBar();

	update_status ImGuiMenuBar();
	void NewScene();
	void PrimitiveMenuItem();

	Tab* GetTab(TabType type);
	int GetStyle() {return style;};
	GameObject* GetGameObjectSelected();

	void SetStyle(int _style);
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

	std::string* GetCurrentFolder();
	Asset* GetSelectedAsset();
	void DestroySelectedAsset();

private:
	std::vector<Tab*> tabs;

	bool showDemoWindow = false;
	bool warningTab = false;

	std::vector<float> fpsLog;
	std::vector<float> msLog;

	bool updateAssets = false;

	std::string* currentFolder;
	
	int style = 5;
	float alphaStyle = 0.1;
	float incrementAlphaValue = 0.1f;
	std::vector<std::string> stylesList;

	float viewportCorSize;

	ImGuiID dockspace_id;
	ImVec4 colorStyle;
};