#pragma once
#include "BasePanel.h"
#include "Graphics/ProfilerTimeline.h"
#include "Helios/Core/Profiler.h"

namespace Helios
{
	class ProfilerPanel : public Editor::IPanel
	{
	public:

		ProfilerPanel()
		{
			title = "Profiler";
			m_custom_begin = true;
		}

		void OnUpdate() override
		{
			if (ImGui::Begin("Profiler", &this->m_window_open))
			{
				HL_PROFILE_BEGIN("GUI - Profiler");
				bool isProfilingThisFrame = Profiler::isProfiling();

				ImGui::Text("FPS: %f", Time::getFPS());

				for (auto scene : SceneRegistry::GetActiveScenes())
				{
					ImGui::Text("%s: %d", scene->GetName().c_str(), scene->GetEntityCount());
				}

				if (ImGui::Checkbox("Profiler Enabled", &isProfilingThisFrame))
				{
					HL_PROFILE_TOGGLE_PROFILING();
				}
				ImGui::SameLine();

				static bool slow_mode = true;
				static float update_interval = 3.0f;
				static Profiler::FrameProfile selectedFrameProfile(0);

				if (ImGui::Button("Clear")) { Profiler::clear(); selectedFrameProfile = (0); }

				if (Profiler::isProfiling())
				{
					if (slow_mode)
					{
						static float tmr = update_interval;
						tmr += Time::deltaTime();

						if (tmr >= update_interval)
						{
							if (Profiler::getResults().size() > 1) selectedFrameProfile = Profiler::getResults()[Profiler::getResults().size() - 2];
							tmr = 0.0f;
						}
					}
					else if (Profiler::getResults().size() > 1) selectedFrameProfile = Profiler::getResults()[Profiler::getResults().size() - 2];
				}



				if (selectedFrameProfile.frameTime)
				{
					uint32_t selectedFrame = 0u;
					unsigned long long frametime = selectedFrameProfile.frameTime / 1000.0f;

					ImGui::Text((std::to_string(Profiler::getResults().size() - 1) + " Frames!").c_str());
					ImGui::Text(("Last frame time: " + std::to_string(selectedFrameProfile.frameTime / 1000.0f) + "ms.").c_str());

					ImGui::PlotLines("Frametime", [](void* data, int idx) { return (Profiler::getResults()[min(max(Profiler::getResults().size() - 300, 0) + idx, Profiler::getResults().size() - 1)].frameTime / 1000.0f); }, nullptr, min(Profiler::getResults().size() - 1, 299), 0, 0, FLT_MAX, FLT_MAX, ImVec2(1800, 150));


					ImGui::Checkbox("Slow Mode", &slow_mode);
					ImGui::SameLine();
					ImGui::DragFloat("Update Interval", &update_interval);

					HL_PROFILE_BEGIN("GUI - Profiler DRAW");
					ProfilerTimeline::Draw(selectedFrameProfile);
					HL_PROFILE_END();

					/*for (auto x : selectedFrameProfile.results)
					{
						ImGui::Button(x.Name, ImVec2(0, 0));
					}*/
				}

				HL_PROFILE_END();
			}
			ImGui::End();
		}
	};
}
