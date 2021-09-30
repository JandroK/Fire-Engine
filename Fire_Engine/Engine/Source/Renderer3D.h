#pragma once
#include "Module.h"
#include "Globals.h"
#include "glmath.h"
#include "Light.h"


#define MAX_LIGHTS 8

class Hardware {
public:
	std::string caps;
	char sdlVersion[25] = "";

	uint cpuCount;
	uint cpuCache;
	float systemRAM;

	uint gpuVendor = 0;
	uint gpuDevice = 0;
	char gpuBrand[250] = "";
	float vramBudget = 0.f;
	float vramUsage = 0.f;
	float vramAvailable = 0.f;
	float vramReserved = 0.f;
};

class Renderer3D : public Module
{
public:
	Renderer3D(Application* app, bool start_enabled = true);
	~Renderer3D();

	bool Init();
	update_status PreUpdate(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

	void GetCaps(std::string& caps);
	void OnResize(int width, int height);
	void OnGUI() override;

public:

	Light lights[MAX_LIGHTS];
	SDL_GLContext context;
	mat3x3 NormalMatrix;
	mat4x4 ModelMatrix, ViewMatrix, ProjectionMatrix;
	bool vsync;

private:
	Hardware hardware;

};