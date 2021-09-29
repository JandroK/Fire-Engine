//#include "Configuration.h"
//#include "Application.h"
//
//Configuration::Configuration(Application* app, bool start_enabled)
//{
//	fpsLog.reserve(FPS_MS_LOG_MAXLENGHT);
//	msLog.reserve(FPS_MS_LOG_MAXLENGHT);
//}
//
//Configuration::~Configuration()
//{
//	msLog.clear();
//	fpsLog.clear();
//}
//
//
//
//update_status Configuration::Update(float dt)
//{
//
//	update_status ret = UPDATE_CONTINUE;
//
//	float currentFPS = floorf(App->GetFrameRate())/*ImGui::GetIO().Framerate*/;
//	float currentMS = (1000.f * App->GetDt());
//
//	if (fpsLog.size() <= FPS_MS_LOG_MAXLENGHT) fpsLog.push_back(currentFPS);
//	else
//	{
//		fpsLog.erase(fpsLog.begin());
//		fpsLog.push_back(currentFPS);
//	}
//	if (msLog.size() <= FPS_MS_LOG_MAXLENGHT) msLog.push_back(currentMS);
//	else
//	{
//		msLog.erase(msLog.begin());
//		msLog.push_back(currentMS);
//	}
//
//
//	return ret;
//}
//
//update_status Configuration::Draw(float dt)
//{
//	if (ImGui::Begin("Configuration"))
//	{
//		if (ImGui::CollapsingHeader("Application"))
//		{
//			ImGui::InputText("App Name", "Fire Engine", 12);
//			ImGui::InputText("Organization", "UPC CITM", 9);
//			ImGui::SliderInt("Max FPS", &App->maxFPS, 0, 144);
//			ImGui::TextWrapped("Limit Framerate: ");
//			ImGui::SameLine();
//			// If FPS is zero, change text FPS to VSync (if fps == 0, FPS = Screen Refresh)
//			ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), (App->maxFPS == 0) ? "VSync" : "%d", App->maxFPS);
//
//			char title[25];
//			sprintf_s(title, 25, "Framerate %.1f", fpsLog[fpsLog.size() - 1]);
//			ImGui::PlotHistogram("##frameRate", &fpsLog[0], fpsLog.size(), 0, title, 0.0f, 100.0f, ImVec2(310, 100));
//			sprintf_s(title, 25, "Milliseconds %0.1f", msLog[msLog.size() - 1]);
//			ImGui::PlotHistogram("##miliseconds", &msLog[0], msLog.size(), 0, title, 0.0f, 100.0f, ImVec2(310, 100));
//			ImGui::NewLine();
//		}
//	}
//	ImGui::End();
//
//	return UPDATE_CONTINUE;
//}