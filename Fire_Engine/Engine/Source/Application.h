#pragma once
#include <iostream>
#include <vector>
#include <list>

#include "Globals.h"
#include "Timer.h"
#include "Module.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleRenderer3D.h"
#include "ModuleCamera3D.h"
#include "ModulePhysics3D.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl.h"
#include "imgui/imgui_impl_opengl2.h"
#include "SDL/include/SDL_opengl.h"

class Application
{
public:
	ModuleWindow* window;
	ModuleCamera3D* camera;
	ModuleInput* input;
	ModuleRenderer3D* renderer3D;
	ModulePhysics3D* physics;

	std::vector<Module*> list_modules;
private:

	Timer	ms_timer;
	float	dt;

public:

	Application();
	~Application();

	bool Init();
	update_status Update();
	bool CleanUp();

private:

	void AddModule(Module* mod);
	void PrepareUpdate();
	void FinishUpdate();

	// Our state
	bool show_demo_window = false;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	ImGuiIO io; 

};