#pragma once
#include "Module.h"
#include "Globals.h"

#include "Tab.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl.h"
#include "imgui/imgui_impl_opengl2.h"

#define FPS_MS_LOG_MAXLENGHT 100

enum class TabType {

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
	
	update_status PreUpdate(float dt);
	update_status Update(float dt);
	update_status PostUpdate(float dt);

	update_status ImGuiMenu();

	bool CleanUp();

private:
	std::vector<Tab*> tabs;

	bool show_demo_window = false;

	std::vector<float> fpsLog;
	std::vector<float> msLog;

	bool showCase;
};