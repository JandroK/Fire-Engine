#pragma once
#include "Module.h"
#include "Globals.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl.h"
#include "imgui/imgui_impl_opengl2.h"

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

	bool CleanUp();

private:
	bool show_demo_window = false;
	ImGuiIO io;


};