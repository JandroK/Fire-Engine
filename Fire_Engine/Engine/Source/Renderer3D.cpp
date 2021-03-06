#include "Application.h"
#include "Globals.h"

// Module
#include "Renderer3D.h"
#include "Window.h"
#include "Camera3D.h"
#include "Editor.h"
#include "Scene.h"
#include "Physics3D.h"

#include <gl/GL.h>
#include <gl/GLU.h>

#include "GPUDetected/DeviceId.h"

// Components
#include "ResourceTexture.h"
#include "MeshRenderer.h"
#include "Transform.h"

#include "AboutTab.h"

#pragma comment (lib, "glu32.lib")    /* link OpenGL Utility lib     */
#pragma comment (lib, "opengl32.lib") /* link Microsoft OpenGL lib   */

Renderer3D::Renderer3D(Application* app, bool start_enabled) : Module(app, start_enabled), vsync(false)
{
	name = "Renderer3D";

	GetCaps(hardware.caps);

	SDL_version version;
	SDL_GetVersion(&version);
	hardware.SDLVersion = std::to_string(version.major) + '.' + std::to_string(version.minor) + '.' + std::to_string(version.patch);
	hardware.CPUCount = SDL_GetCPUCount();
	hardware.CPUCache = SDL_GetCPUCacheLineSize();
	hardware.systemRAM = SDL_GetSystemRAM() / 1024.f;	

	uint vendor, deviceId;
	std::wstring brand;
	unsigned __int64 videoMemBudget;
	unsigned __int64 videoMemUsage;
	unsigned __int64 videoMemAvailable;
	unsigned __int64 videoMemReserved;

	if (getGraphicsDeviceInfo(&vendor, &deviceId, &brand, &videoMemBudget, &videoMemUsage, &videoMemAvailable, &videoMemReserved))
	{
		hardware.GPUVendor = vendor;
		hardware.GPUDevice = deviceId;
		sprintf_s(hardware.GPUBrand, 250, "%S", brand.c_str());
		hardware.VRAMBudget = float(videoMemBudget) / (1073741824.0f / 1024.f);
		hardware.VRAMUsage = float(videoMemUsage) / (1024.f * 1024.f * 1024.f);
		hardware.VRAMAvailable = float(videoMemAvailable) / (1024.f * 1024.f);
		hardware.VRAMReserved = float(videoMemReserved) / (1024.f * 1024.f * 1024.f);
	}
}

// Called before render is available
bool Renderer3D::Init()
{
	LOG(LogType::L_NORMAL, "Creating 3D Renderer context");
	bool ret = true;
	
	//Create context
	context = SDL_GL_CreateContext(App->window->window);
	if(context == NULL)
	{
		LOG(LogType::L_ERROR, "OpenGL context could not be created! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}

	GLenum error = glewInit();
	if (error != GL_NO_ERROR)
	{
		LOG(LogType::L_ERROR, "Error initializing glew library! %s", SDL_GetError());
		ret = false;
	}
	else
	{
		LOG(LogType::L_NORMAL, "Init Glew");
	}
	
	if(ret == true)
	{
		// Print version info
		static_cast<AboutTab*>(app->editor->GetTab(TabType::ABOUT))->LogVersionDependences();

		//Use Vsync
		if(VSYNC && SDL_GL_SetSwapInterval(static_cast<int>(vsync)) < 0)
			LOG(LogType::L_ERROR, "Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());

		//Initialize Projection Matrix
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		//Check for error
		GLenum error = glGetError();
		if(error != GL_NO_ERROR)
		{
			LOG(LogType::L_ERROR, "Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}

		//Initialize Modelview Matrix
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		//Check for error
		error = glGetError();
		if(error != GL_NO_ERROR)
		{
			LOG(LogType::L_ERROR, "Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}
		
		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
		glClearDepth(1.0f);
		
		//Initialize clear color
		glClearColor(0.10f, 0.10f, 0.10f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT);

		//Check for error
		error = glGetError();
		if(error != GL_NO_ERROR)
		{
			LOG(LogType::L_ERROR, "Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}

		// Blend for transparency
		glEnable(GL_BLEND);
		glBlendEquation(GL_FUNC_ADD);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		GLfloat LightModelAmbient[] = {0.0f, 0.0f, 0.0f, 1.0f};
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, LightModelAmbient);
		
		lights[0].ref = GL_LIGHT0;
		lights[0].ambient.Set(0.25f, 0.25f, 0.25f, 1.0f);
		lights[0].diffuse.Set(0.75f, 0.75f, 0.75f, 1.0f);
		lights[0].SetPos(0.0f, 0.0f, 2.5f);
		lights[0].Init();
		
		GLfloat MaterialAmbient[] = {1.0f, 1.0f, 1.0f, 1.0f};
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, MaterialAmbient);

		GLfloat MaterialDiffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, MaterialDiffuse);
		
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		lights[0].Active(true);
		glEnable(GL_LIGHTING);
		glEnable(GL_COLOR_MATERIAL);
		glEnable(GL_TEXTURE_2D);
	}

	//Generate texture
	for (int i = 0; i < SQUARE_TEXTURE_W; i++) {
		for (int j = 0; j < SQUARE_TEXTURE_H; j++) {
			int c = ((((i & 0x8) == 0) ^ (((j & 0x8)) == 0))) * 255;
			checkerImage[i][j][0] = (GLubyte)c;
			checkerImage[i][j][1] = (GLubyte)c;
			checkerImage[i][j][2] = (GLubyte)c;
			checkerImage[i][j][3] = (GLubyte)255;
		}
	}

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &checkersTexture);
	glBindTexture(GL_TEXTURE_2D, checkersTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SQUARE_TEXTURE_W, SQUARE_TEXTURE_H, 0, GL_RGBA, GL_UNSIGNED_BYTE, checkerImage);
	glBindTexture(GL_TEXTURE_2D, 0);

	App->camera->Move(float3(1.0f, 1.0f, 0.0f));
	App->camera->LookAt(float3(0, 0, 0));

	// Projection matrix for
	OnResize(app->window->GetWindowWidth(), app->window->GetWindowHeight());

	plane.SetPos(0, 0, 0);
	plane.constant = 0;
	plane.axis = true;
	return ret;
}

// PreUpdate: clear buffer
update_status Renderer3D::PreUpdate(float dt)
{
	app->camera->cameraScene.PreUpdate();

	// light 0 on cam pos
	lights[0].SetPos(App->camera->position.x, App->camera->position.y, App->camera->position.z);

	for(uint i = 0; i < MAX_LIGHTS; ++i)
		lights[i].Render();

	return UPDATE_CONTINUE;
}

// PostUpdate present buffer to screen
update_status Renderer3D::PostUpdate(float dt)
{
	update_status ret = UPDATE_CONTINUE;
	
	plane.Render();

	// Comprobe wireframe mode
	(wireframe) ? glPolygonMode(GL_FRONT_AND_BACK, GL_LINE) : glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	(wireframe) ? glColor3f(Yellow.r, Yellow.g, Yellow.b) : glColor3f(White.r, White.g, White.b);
	
	// Draw all meshes
	if (!renderQueue.empty())
	{
		for (size_t i = 0; i < renderQueue.size(); i++)
		{
			// Frustrum Culling
			if (app->camera->cameraScene.frustrum.Intersects(renderQueue[i]->globalOBB))
				renderQueue[i]->DrawMesh();
		}
		app->camera->DrawSpacePartition();
	}

	// Draw the Ray
	DrawRay();

	app->camera->cameraScene.PostUpdate();

	// Game Camera
	if (app->scene->mainCamera != nullptr)
	{
		app->scene->mainCamera->PreUpdate();

		// light 0 on cam pos
		lights[0].SetPos(App->camera->position.x, App->camera->position.y, App->camera->position.z);

		for (uint i = 0; i < MAX_LIGHTS; ++i)
			lights[i].Render();

		app->physics->PostUpdate(dt);

		plane.Render();

		// Draw all meshes
		if (!renderQueue.empty())
		{
			for (size_t i = 0; i < renderQueue.size(); i++)
			{
				// Frustrum Culling
				if(app->scene->mainCamera->frustrum.Intersects(renderQueue[i]->globalOBB))
					renderQueue[i]->DrawMesh();
			}
		}

		app->scene->mainCamera->PostUpdate();
	}


	glDisable(GL_DEPTH_TEST);
	glClearColor(0.10f, 0.10f, 0.10f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Draw all tabs
	ret = app->editor->Draw();

	renderQueue.clear();
	SDL_GL_SwapWindow(app->window->window);

	(wireframe) ? glPolygonMode(GL_FRONT_AND_BACK, GL_FILL) : glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	return ret;
}

void Renderer3D::DrawRay()
{
	if (viewRay)
	{
		glColor3f(1.f, 0.f, 0.f);
		glLineWidth(2.f);
		glBegin(GL_LINES);
		glVertex3fv(&app->camera->ray.a.x);
		glVertex3fv(&app->camera->ray.b.x);
		glEnd();
		glLineWidth(1.f);
		glColor3f(1.f, 1.f, 1.f);
	}
}

// Called before quitting
bool Renderer3D::CleanUp()
{
	LOG(LogType::L_NO_PRINTABLE, "Destroying 3D Renderer");
		
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glDisable(GL_LIGHTING);
	glDisable(GL_COLOR_MATERIAL);
	glDisable(GL_TEXTURE_2D);

	RELEASE(plane.mesh);

	renderQueue.clear();

	SDL_GL_DeleteContext(context);

	return true;
}


void Renderer3D::GetCaps(std::string& caps)
{
	caps += (SDL_HasRDTSC()) ? "RDTSC," : "";
	caps += (SDL_HasMMX()) ? "MMX, " : "";
	caps += (SDL_HasSSE()) ? "SSE, " : "";
	caps += (SDL_HasSSE2()) ? "SSE2, " : "";
	caps += (SDL_HasSSE3()) ? "SSE3, " : "";
	caps += "\n";
	caps += (SDL_HasSSE41()) ? "SSE41, " : "";
	caps += (SDL_HasSSE42()) ? "SSE42, " : "";
	caps += (SDL_HasAVX()) ? "AVX, " : "";
	caps += (SDL_HasAltiVec()) ? "AltiVec, " : "";
	caps += (SDL_Has3DNow()) ? "3DNow, " : "";
}

void Renderer3D::OnResize(int width, int height)
{
	glViewport(0, 0, width, height);

	app->camera->cameraScene.ReGenerateFrameBuffer(width, height);
	if(app->scene->mainCamera != nullptr)
		app->scene->mainCamera->ReGenerateFrameBuffer(width, height);
}

void Renderer3D::OnGUI()
{
	if (ImGui::CollapsingHeader("Hardware"))
	{
		IMGUI_PRINT("SDL Version: ", "%s", hardware.SDLVersion.c_str());
		IMGUI_PRINT("OpenGL Version: ", "%s", glGetString(GL_VERSION));
		IMGUI_PRINT("Glew Version: ", "%s", glewGetString(GLEW_VERSION));

		ImGui::Separator();
		IMGUI_PRINT("GLSL: ", "%s", glGetString(GL_SHADING_LANGUAGE_VERSION));
		IMGUI_PRINT("Vendor: ", "%s", glGetString(GL_VENDOR));
		IMGUI_PRINT("Renderer: ", "%s", glGetString(GL_RENDERER));

		//ImGui::TextWrapped("All external library versions can be found in the 'About' window with links to their pages.");

		ImGui::Separator();
		IMGUI_PRINT("CPUs: ", "%d (Cache: %dkb)", hardware.CPUCount, hardware.CPUCache);
		IMGUI_PRINT("System RAM: ", "%.1fGb", hardware.systemRAM);
		IMGUI_PRINT("Caps: ", hardware.caps.c_str());

		ImGui::Separator();
		IMGUI_PRINT("GPU:", "vendor %u device %u", hardware.GPUVendor, hardware.GPUDevice);
		IMGUI_PRINT("Brand:", hardware.GPUBrand);
		IMGUI_PRINT("VRAM Budget:", "%.1f Mb", hardware.VRAMBudget);
		IMGUI_PRINT("VRAM Usage:", "%.1f Mb", hardware.VRAMUsage);
		IMGUI_PRINT("VRAM Available:", "%.1f Mb", hardware.VRAMAvailable);
		IMGUI_PRINT("VRAM Reserved:", "%.1f Mb", hardware.VRAMReserved);
	}
	if (ImGui::CollapsingHeader("Debug"))
	{
		GlDebugMode();
	}
}

void Renderer3D::GlDebugMode()
{
	if (ImGui::Checkbox("GL_DEPTH_TEST", &depthTest)) {
		if (depthTest) glEnable(GL_DEPTH_TEST);
		else glDisable(GL_DEPTH_TEST);
	}

	if (ImGui::IsItemHovered())
		ImGui::SetTooltip("Enable/Disable GL_DEPTH_TEST");

	ImGui::SameLine();

	if (ImGui::Checkbox("GL_CULL_FACE", &cullFace)) {
		if (cullFace) glEnable(GL_CULL_FACE);
		else glDisable(GL_CULL_FACE);
	}

	if (ImGui::IsItemHovered())
		ImGui::SetTooltip("Enable/Disable GL_CULL_FACE");


	if (ImGui::Checkbox("GL_TEXTURE_2D", &texture2D)) {
		if (texture2D) glEnable(GL_TEXTURE_2D);
		else glDisable(GL_TEXTURE_2D);
	}

	if (ImGui::IsItemHovered())
		ImGui::SetTooltip("Enable/Disable GL_TEXTURE_2D");

	ImGui::SameLine();

	if (ImGui::Checkbox("GL_LIGHTING", &lighting)) {
		if (lighting) glEnable(GL_LIGHTING);
		else glDisable(GL_LIGHTING);
	}

	if (ImGui::IsItemHovered())
		ImGui::SetTooltip("Enable/Disable GL_LIGHTING");

	if (ImGui::Checkbox("RAY_CASTING  ", &viewRay))

		if (ImGui::IsItemHovered())
			ImGui::SetTooltip("Enable/Disable can view ray");

	ImGui::SameLine();
	if (ImGui::Checkbox("GL_COLOR_MATERIAL", &colorMaterial)) {
		if (colorMaterial) glEnable(GL_COLOR_MATERIAL);
		else glDisable(GL_COLOR_MATERIAL);
	}

	if (ImGui::IsItemHovered())
		ImGui::SetTooltip("Enable/Disable GL_COLOR_MATERIAL");
}

bool Renderer3D::SaveConfig(JsonParser& node) const
{
	node.SetJBool(node.ValueToObject(node.GetRootValue()), "vsync", vsync);
	node.SetJBool(node.ValueToObject(node.GetRootValue()), "wireframe", wireframe);

	return true;
}

bool Renderer3D::LoadConfig(JsonParser& node)
{
	vsync = node.JsonValToBool("vsync");
	wireframe = node.JsonValToBool("wireframe");

	return true;
}