#include "AboutTab.h"
#include "Globals.h"

#include "imgui/imgui.h"
#include "imgui/imgui_user.h"
#include "imgui/imgui_impl_sdl.h"
#include "imgui/imgui_impl_opengl3.h"

#include"PhysFS/include/physfs.h"
#include "Assimp/include/version.h"

#include "SDL/include/SDL.h"
#include "Glew/include/glew.h"

AboutTab::AboutTab()
{
	name = "About";
	active = false;

	PHYSFS_Version physVersionStc;
	PHYSFS_getLinkedVersion(&physVersionStc);
	SDL_version version;
	SDL_GetVersion(&version);

	SDLVersion = std::to_string(version.major) + '.' + std::to_string(version.minor) + '.' + std::to_string(version.patch);
	physVersion = std::to_string(physVersionStc.major) + '.' + std::to_string(physVersionStc.minor) + '.' + std::to_string(physVersionStc.patch);
	assimpVersion = std::to_string(aiGetVersionMajor()) + '.' + std::to_string(aiGetVersionMinor()) + '.' + std::to_string(aiGetVersionRevision());
}

void AboutTab::Draw()
{
	ImGui::Text("Fire Engine");
	ImGui::Text("This is a university project focused");
	ImGui::Text("on the development of a video game engine");

	ImGui::NewLine();
	ImGui::Text("Developed by: ");
	if (ImGui::MenuItem("Ismael Tejada"))
	{
		ShellExecute(0, 0, "https://github.com/IsmaUPC", 0, 0, SW_SHOW);
	}
	if (ImGui::MenuItem("Alejandro Moreno"))
	{
		ShellExecute(0, 0, "https://github.com/JandroK", 0, 0, SW_SHOW);
	}

	ImGui::NewLine();
	ImGui::Separator();

	ImGui::Text("3rd Party Libraries used:");
	IMGUI_PRINT("SDL Version: ", "%s", SDLVersion.c_str());
	IMGUI_PRINT("OpenGL Version: ", "%s", glGetString(GL_VERSION));
	IMGUI_PRINT("Glew Version: ", "%s", glewGetString(GLEW_VERSION));
	IMGUI_PRINT("ImGui Version: ", "%s", ImGui::GetVersion());
	IMGUI_PRINT("MathGeoLib Version: ", "1.5");
	IMGUI_PRINT("Parson Version: ", "1.2.1");
	IMGUI_PRINT("Assimp Version: ", "%s", assimpVersion.c_str());
	IMGUI_PRINT("PhysFS Version: ", "%s", physVersion.c_str());
	IMGUI_PRINT("DeviL Version: ", "1.8");

	ImGui::NewLine();
	ImGui::Separator();

	PrintLicense();
}

void AboutTab::PrintLicense()
{
	ImGui::Text("License:");
	ImGui::Text("MIT License \nCopyright(c) 2021 JandroK \n");
	ImGui::NewLine();
	ImGui::Text("Permission is hereby granted, free of charge, to any person obtaining a copy");
	ImGui::Text("of this softwareand associated documentation files (the \"Software\"), to deal");
	ImGui::Text("in the Software without restriction, including without limitation the rights");
	ImGui::Text("to use, copy, modify, merge, publish, distribute, sublicense, and /or sell");
	ImGui::Text("copies of the Software, and to permit persons to whom the Software is");
	ImGui::Text("furnished to do so, subject to the following conditions:");
	ImGui::NewLine();
	ImGui::Text("The above copyright noticeand this permission notice shall be included in all");
	ImGui::Text("copies or substantial portions of the Software.");
	ImGui::NewLine();
	ImGui::Text("THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR");
	ImGui::Text("IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,");
	ImGui::Text("FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE");
	ImGui::Text("AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER");
	ImGui::Text("LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,");
	ImGui::Text("OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE");
	ImGui::Text("SOFTWARE");
}
