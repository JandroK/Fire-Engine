#pragma once
#include "Module.h"
#include "Globals.h"
#include "../ModuleWindow.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl.h"
#include "imgui/imgui_impl_opengl2.h"

#define FPS_MS_LOG_MAXLENGHT 100

class ModuleEditor:public Module
{
public:
	ModuleEditor(Application* app, bool start_enabled = true);

	~ModuleEditor();
	bool Init();

	bool Start();
	
	update_status PreUpdate(float dt);
	update_status Update(float dt);
	update_status PostUpdate(float dt);

	update_status ImGuiMenu();
	void ImGuiFPSGraph();

	bool CleanUp();

private:
	std::vector<ModuleWindow*> windows;

	bool show_demo_window = false;
	ImGuiIO io;

	bool showCase;
	std::vector<float> fps_log;
	std::vector<float> ms_log;

};