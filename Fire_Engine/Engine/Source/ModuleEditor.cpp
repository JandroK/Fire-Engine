#include "Application.h"
#include "Globals.h"
#include "ModuleEditor.h"

ModuleEditor::ModuleEditor(Application* app, bool start_enabled): Module(app, start_enabled)
{
	fps_log.reserve(FPS_MS_LOG_MAXLENGHT);
	ms_log.reserve(FPS_MS_LOG_MAXLENGHT);
}

ModuleEditor::~ModuleEditor()
{
	ms_log.clear();
	fps_log.clear();
}

bool ModuleEditor::Init()
{
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	io = ImGui::GetIO();
	(void)io;

	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;    // Enable Gamepad Controls

	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();

	// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	// Setup Platform/Renderer backends
	ImGui_ImplSDL2_InitForOpenGL(App->window->window, App->window->gl_context);
	ImGui_ImplOpenGL2_Init();

    return true;
}

bool ModuleEditor::Start()
{
    return true;
}

update_status ModuleEditor::PreUpdate(float dt)
{
    return UPDATE_CONTINUE;
}

update_status ModuleEditor::Update(float dt)
{

	update_status ret = UPDATE_CONTINUE;
	// Start the Dear ImGui frame
	ImGui_ImplOpenGL2_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();

	float currentFPS = floorf(App->GetFrameRate())/*ImGui::GetIO().Framerate*/;
	float currentMS = (1000.f * App->GetDt());

	if (fps_log.size() <= FPS_MS_LOG_MAXLENGHT) fps_log.push_back(currentFPS);
	else
	{
		fps_log.erase(fps_log.begin());
		fps_log.push_back(currentFPS);
	}
	if (ms_log.size() <= FPS_MS_LOG_MAXLENGHT) ms_log.push_back(currentMS);
	else
	{
		ms_log.erase(ms_log.begin());
		ms_log.push_back(currentMS);
	}

	ret = ImGuiMenu();
	ImGuiFPSGraph();


    return ret;
}

update_status ModuleEditor::PostUpdate(float dt)
{	
	
	// Rendering
	ImGui::Render();
	glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
	//glUseProgram(0); // You may want this if using this code in an OpenGL 3+ context where shaders may be bound
	ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());


	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		SDL_Window* backup_current_window = SDL_GL_GetCurrentWindow();
		SDL_GLContext backup_current_context = SDL_GL_GetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		SDL_GL_MakeCurrent(backup_current_window, backup_current_context);
	}

	SDL_GL_SwapWindow(App->window->window);
    return UPDATE_CONTINUE;
}

update_status ModuleEditor::ImGuiMenu()
{
	update_status ret = update_status::UPDATE_CONTINUE;
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Close ESC"))
				ret = UPDATE_STOP;
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("View"))
		{
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Help"))
		{

			if (ImGui::MenuItem("ImGui Demo", nullptr, showCase))
			{
				showCase = !showCase;
			}
			if (ImGui::MenuItem("Documentation"))
			{
				ShellExecute(0, 0, "https://github.com/JandroK/Fire-Engine/wiki", 0, 0, SW_SHOW);
			}
			if (ImGui::MenuItem("Download latest"))
			{
				ShellExecute(0, 0, "https://github.com/JandroK/Fire-Engine/releases", 0, 0, SW_SHOW);
			}
			if (ImGui::MenuItem("Report a bug"))
			{
				ShellExecute(0, 0, "https://github.com/JandroK/Fire-Engine/issues", 0, 0, SW_SHOW);
			}
			if (ImGui::BeginMenu("About"))
			{
				if (ImGui::MenuItem("Ismael Tejada"))
				{
					ShellExecute(0, 0, "https://github.com/IsmaUPC", 0, 0, SW_SHOW);
				}
				if (ImGui::MenuItem("Alejandro Moreno"))
				{
					ShellExecute(0, 0, "https://github.com/JandroK", 0, 0, SW_SHOW);
				}
				ImGui::EndMenu();
			}			
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}
	return ret;
}

void ModuleEditor::ImGuiFPSGraph()
{
	if (ImGui::Begin("Configuration"))
	{
		if (ImGui::CollapsingHeader("Application"))
		{
			ImGui::InputText("App Name", "Fire Engine",12);
			ImGui::InputText("Organization", "UPC CITM", 9);
			ImGui::SliderInt("Max FPS", &App->maxFPS, 0, 144);
			ImGui::TextWrapped("Limit Framerate: ");
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "%d", App->maxFPS);

			char title[25];
			sprintf_s(title, 25, "Framerate %.1f", fps_log[fps_log.size()-1]);
			ImGui::PlotHistogram("##frameRate", &fps_log[0], fps_log.size(), 0, title, 0.0f, 100.0f, ImVec2(310, 100));
			sprintf_s(title, 25, "Milliseconds %0.1f", ms_log[ms_log.size() - 1]);
			ImGui::PlotHistogram("##miliseconds", &ms_log[0], ms_log.size(), 0, title, 0.0f, 100.0f, ImVec2(310, 100));
			ImGui::NewLine();
		}
	}
	ImGui::End();
}

bool ModuleEditor::CleanUp()
{
	ImGui_ImplOpenGL2_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

    return false;
}
