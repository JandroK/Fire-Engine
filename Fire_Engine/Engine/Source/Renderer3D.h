#pragma once
#include "Module.h"
#include "glmath.h"
#include "Light.h"
#include "Primitive.h"

#include "Glew/include/glew.h"
#include "SDL\include\SDL_opengl.h"
#include "SDL\include\SDL.h"

class Texture;
class MeshRenderer;

#define MAX_LIGHTS 8
#define SQUARE_TEXTURE_W 256
#define SQUARE_TEXTURE_H 256

class Hardware {
public:
	std::string caps;
	std::string SDLVersion;

	uint CPUCount;
	uint CPUCache;
	float systemRAM;

	uint GPUVendor = 0;
	uint GPUDevice = 0;
	char GPUBrand[250] = "";
	float VRAMBudget = 0.f;
	float VRAMUsage = 0.f;
	float VRAMAvailable = 0.f;
	float VRAMReserved = 0.f;
};

class Renderer3D : public Module
{
public:
	Renderer3D(Application* app, bool start_enabled = true);
	~Renderer3D() {};

	bool Init()override;
	update_status PreUpdate(float dt)override;
	update_status PostUpdate(float dt)override;
	void DrawRay();
	bool CleanUp();

	void GetCaps(std::string& caps);
	void OnResize(int width, int height);
	void OnGUI() override;

	void GlDebugMode();

	bool SaveConfig(JsonParser& node) const override;

	bool LoadConfig(JsonParser& node)override;

	Hardware GetHardware() { return hardware; };

public:

	Light lights[MAX_LIGHTS];
	SDL_GLContext context;

	bool vsync;
	bool wireframe = false;

	// Textures
	GLuint checkersTexture;
	GLubyte checkerImage[SQUARE_TEXTURE_W][SQUARE_TEXTURE_H][4];

	std::vector<MeshRenderer*> renderQueue;

private:
	PPlane plane;

	Hardware hardware;
	bool depthTest = true;
	bool cullFace = true;
	bool viewRay = true;
	bool lighting = true;
	bool colorMaterial = true;
	bool texture2D = true;
};