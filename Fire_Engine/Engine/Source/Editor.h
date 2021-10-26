#pragma once
#include "Module.h"
#include "Globals.h"

#include "imgui/imgui.h"
#include "imgui/imgui_user.h"
#include "imgui/imgui_impl_sdl.h"
#include "imgui/imgui_impl_opengl3.h"

class Tab;
#define FPS_MS_LOG_MAXLENGHT 100

enum class TabType {

	ABOUT,
	CONSOLE,
	CONFIGURATION,

	MAX
};

class Editor:public Module
{
public:
	Editor(Application* app, bool start_enabled = true);

	~Editor();
	bool Init();

	bool Start();
	void LogToConsole(const char* msg, LogType _type);
	
	update_status PreUpdate(float dt);
	update_status Update(float dt);
	update_status PostUpdate(float dt);

	update_status ImGuiMenuBar();
	Tab* GetTab(TabType type);

	bool CleanUp();

	void CreateDockSpace();
	ImGuiID DockSpaceOverViewportCustom(ImGuiViewport* viewport, ImGuiDockNodeFlags dockspaceFlags, ImVec2 position, ImVec2 size, const ImGuiWindowClass* windowClass);

private:
	std::vector<Tab*> tabs;

	bool show_demo_window = false;

	std::vector<float> fpsLog;
	std::vector<float> msLog;

	bool showCase = false;

	// DockingSpace
	ImGuiID dockId = 0;
};