#include "Toolbar.h"

#include "Application.h"
#include "pch.h"

#include "imgui.h"
#include "ProjectManager.h"
#include "Helios/Scene/SceneSerializer.h"
// #include "Panels/InspectorPanel.h"

#include "Helios/Utils/Conversions.h"

#include <GLFW/glfw3.h>

#ifdef HELIOS_PLATFORM_WINDOWS
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#endif
namespace Helios
{
	void DrawToolbar(EditorMode &mode, std::vector<Editor::IPanel *> &panels)
	{

		static uint32_t titlebar_size = 40u;
		static uint32_t titlebar_between_controls = 5u;

		// static uint32_t titlebar_control_padding = 5u;
		// static ImVec2 control_size = ImVec2(40 - 5 * 2, 40 - 5 * 2);
		static ImVec2 control_size = ImVec2(titlebar_size, titlebar_size);

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 15));
		if (ImGui::BeginMenuBar())
		{
			ImGui::PopStyleVar();
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("New Scene", "", false, mode == EditorMode::Editor))
				{
					InspectorPanel::Reset();
					Project::LoadSampleScene();
				}
				if (ImGui::MenuItem("Open Scene", "Ctrl+O", false, mode == EditorMode::Editor))
				{
					InspectorPanel::Reset();
					// Project::OpenSceneDialog();

					#ifdef HELIOS_PLATFORM_WINDOWS
					
					wchar_t file[MAX_PATH];
					ZeroMemory(file, sizeof(MAX_PATH));

					OPENFILENAMEW props;
					ZeroMemory(&props, sizeof(props));
					props.lStructSize = sizeof(props);
					props.hwndOwner = glfwGetWin32Window((GLFWwindow *)Application::GetInstance().GetWindow()->GetNativeWindow());
					props.lpstrFilter = L"Scene (*.scene)\0*.scene\0All Files (*.*)\0*.*\0";
					props.lpstrFile = file;
					props.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;
					props.lpstrDefExt = L"scene";
					props.nMaxFile = MAX_PATH;
					// props.lpstrInitialDir = L"C:\\";
					props.lpstrInitialDir = Project::GetAssetsPath().wstring().c_str();

					if (GetSaveFileName(&props))
					{
						std::wstring wstr(file);
						std::string str = conversions::from_utf16_to_utf8(wstr);
						if (str.ends_with(".scene"))
						{
							SceneRegistry::LoadEmptyScene("Empty");
							SceneSerializer serializer(SceneRegistry::GetActiveScenes()[0]); 

							if (serializer.DeserializeEditor(str))
								MessageBoxA(NULL, "Scene loaded successfully!", "Success!", MB_ICONINFORMATION);
							else
								MessageBoxA(NULL, "Failed to load scene!", "Failed!", MB_ICONERROR);
						}
						else
							MessageBoxA(NULL, (std::string("File must end with .scene extention!") + str).c_str(), "Failed to load scene!", MB_ICONERROR);
					}

					#endif
				}

				ImGui::Separator();

				if (ImGui::MenuItem("Save", "Ctrl+S", false, mode == EditorMode::Editor))
				{
					if (Helios::currentMode == EditorMode::Editor)
					{
						for (auto &scene : SceneRegistry::GetActiveScenes())
						{
							SceneSerializer serializer(scene);
							serializer.SerializeEditor((Project::GetAssetsPath() / "Test.scene").string());
						}
					}
				}

				if (ImGui::MenuItem("Save As...", "Ctrl+Shift+S", false, mode == EditorMode::Editor))
				{
					if (Helios::currentMode == EditorMode::Editor)
					{
						Project::SaveScene(true);
					}
				}

				ImGui::Separator();

				if (ImGui::MenuItem("New Project"))
				{
#ifdef HELIOS_PLATFORM_WINDOWS
					ShellExecuteA(NULL, "open", std::filesystem::current_path().append("ProjectExplorer.exe").string().c_str(), NULL, NULL, SW_SHOWDEFAULT);
#else
					Helios::ShowMessage("Error", "Not implemented!", Helios::Message::IconError);
#endif
				}

				if (ImGui::MenuItem("Open Project"))
				{
#ifdef HELIOS_PLATFORM_WINDOWS
					ShellExecuteA(NULL, "open", std::filesystem::current_path().append("ProjectExplorer.exe").string().c_str(), NULL, NULL, SW_SHOWDEFAULT);
#else
					Helios::ShowMessage("Error", "Not implemented!", Helios::Message::IconError);
#endif
				}

				ImGui::Separator();

				if (ImGui::MenuItem("Exit"))
				{
					Application::GetInstance().Quit();
				}
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Window"))
			{
				// if (ImGui::MenuItem("Profiler")) { show_profiler_window = true; }

				for (auto &panel : panels)
				{
					if (ImGui::MenuItem(panel->GetName().c_str()))
					{
						panel->m_window_open = true;
					}
				}

				if (ImGui::MenuItem("Scene View"))
				{
					panels.push_back(new ScenePanel());
				}

				ImGui::EndMenu();
			}

			ImVec2 pos = ImGui::GetWindowPos();
			ImVec2 size = ImGui::GetWindowSize();

			ImGui::SetNextWindowPos(ImVec2(pos.x + size.x / 2.0f - 50.0f, pos.y));
			// ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(0, 0, 0, 255));
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
			ImGui::BeginChild("statePanel", ImVec2(58, 25));

			ImVec4 col = (mode == EditorMode::Playing ? ImVec4(0.5f, 0.5f, 0.5f, 1.0f) : ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
			if (ImGui::ImageButton("playBtn", mode == EditorMode::Editor ? HeliosEditor::ICON_PLAY_WHITE->GetTextureID() : HeliosEditor::ICON_STOP_WHITE->GetTextureID(), ImVec2(25, 25), ImVec2(0, 0), ImVec2(1, 1), col))
			{
				switch (mode)
				{
				case EditorMode::Editor:
					mode = EditorMode::Playing;
					HeliosEditor::OnEnterPlaymode();
					break;
				case EditorMode::Paused:
				case EditorMode::Playing:
					mode = EditorMode::Editor;
					HeliosEditor::OnExitPlaymode();
					break;
				}
			}
			ImGui::SameLine();
			col = (mode == EditorMode::Paused ? ImVec4(0.5f, 0.5f, 0.5f, 1.0f) : ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
			if (ImGui::ImageButton("pauseBtn", mode == EditorMode::Paused ? HeliosEditor::ICON_PLAY_WHITE->GetTextureID() : HeliosEditor::ICON_PAUSE_WHITE->GetTextureID(), ImVec2(25, 25), ImVec2(0, 0), ImVec2(1, 1), col))
			{
				switch (mode)
				{
				case EditorMode::Paused:
					mode = EditorMode::Playing;
					break;
				case EditorMode::Playing:
					mode = EditorMode::Paused;
					break;
				default: break;
				}
			}

			ImGui::EndChild();
			ImGui::PopStyleVar();
			// ImGui::PopStyleColor();

			// ImGui::ImageButton((ImTextureID)*ICON_PLAY_WHITE, ImVec2(20, 20));
			/*if ()
				mode = EditorMode::Playing;
			}*/
			ImGui::SetCursorPosX(ImGui::GetWindowWidth() - (titlebar_between_controls * 3 + titlebar_size * 3));
			ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyle().Colors[ImGuiCol_MenuBarBg]);
			// ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImGui::GetStyle().Colors[ImGuiCol_MenuBarBg]);
			if (ImGui::Button((const char *)ICON_UI_CONTROL_MINIMIZE, control_size))
			{
				Application::GetInstance().GetWindow()->Minimize();
			}

			bool maximized = Application::GetInstance().GetWindow()->IsMaximized();
			ImGui::SetCursorPosX(ImGui::GetWindowWidth() - (titlebar_between_controls * 2 + titlebar_size * 2));
			if (ImGui::Button(maximized ? (const char*)ICON_UI_CONTROL_RESTORE : (const char*)ICON_UI_CONTROL_MAXIMIZE, control_size))
			{
				if (maximized)
				{
					Application::GetInstance().GetWindow()->Restore();
				}
				else
				{
					Application::GetInstance().GetWindow()->Maximize();
				}
			}

			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32(255, 20, 20, 255));
			ImGui::SetCursorPosX(ImGui::GetWindowWidth() - (titlebar_between_controls + titlebar_size));
			if (ImGui::Button((const char *)ICON_UI_CONTROL_CLOSE, control_size))
			{
				Application::GetInstance().GetWindow()->Close();
			}
			ImGui::PopStyleColor();

			ImGui::PopStyleColor();

			ImGui::EndMenuBar();
		}

		ImGui::SetCursorPosY(ImGui::GetWindowHeight() - 25);
		ImGui::PushStyleColor(ImGuiCol_ChildBg, ImGui::GetStyle().Colors[ImGuiCol_MenuBarBg]);
		if (ImGui::BeginChild("BottomBar", ImVec2(0, 25), true, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_AlwaysUseWindowPadding))
		{
			if (ImGui::BeginMenuBar())
			{
				ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Helios Editor");
				ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Version 1.0.0");

				ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Editing Scene: %s", SceneRegistry::GetActiveScenes()[0]->GetName().c_str());

				ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 100);
				if (ImGui::BeginMenu("Notifications"))
				{
					ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Notifications");

					if (ImGui::MenuItem("Notifications"))
					{
					}

					if (ImGui::Button("WHAT HWTAT"))
					{
					}

					ImGui::EndMenu();
				}

				ImGui::EndMenuBar();
			}
			ImGui::EndChild();
		}
		ImGui::PopStyleColor();
	}
}
