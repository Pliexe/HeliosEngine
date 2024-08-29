#pragma once
#include "BasePanel.h"
#include "Helios/Core/Profiler.h"

namespace Helios
{
	class ProfilerPanel : public Editor::IPanel
	{
	public:

		uint64_t m_inspecting_frame = 0u;

		ProfilerPanel()
		{
			title = "Profiler";
			m_custom_begin = true;
			m_window_open = false;
		}

		void DrawTimline()
		{
			// default > 100 pixels = 1ms
			// 1 pixel = 0.01ms

			if (m_inspecting_frame >= Profiler::getFrameResults().size()) return;

			static float ms_size = 5.f;
			static float cursor_pos = 0.0f;

			ImGui::DragFloat("Zoom - MS", &ms_size, 0.1f, 1.0f, 1000.0f, "%.2f");

			auto& frame = Profiler::getFrameResults()[m_inspecting_frame];
			
			ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(15, 15, 15, 255));
			ImGui::BeginChild("##timeline", ImVec2(0, 400), false, ImGuiWindowFlags_HorizontalScrollbar);


			ImVec2 windowSize = ImGui::GetWindowSize();

			auto contentRegion = ImGui::GetContentRegionMax();

			auto& io = ImGui::GetIO();

			auto test1 = ImGui::IsWindowHovered();
			auto test2 = io.MouseWheel != 0.0f;

			// If mouse is scrolling
			if (ImGui::IsWindowHovered() && io.MouseWheel != 0.0f)
			{
				if (io.KeyShift)
				{
					// scale zoom speed with how much we are zoomed in
					ms_size += io.MouseWheel * ms_size * 0.1f;
				}
				else
				{
					cursor_pos += io.MouseWheel * 10.0f;
				}
			}


			std::vector<Profiler::CpuTaskProfile>& tasks = Profiler::getTaskResults();

			uint32_t start_at = frame.starts_at;

			for (int i = start_at; i > 0; i--)
			{
				if (tasks[i].EndTime < frame.startTime && (frame.starts_at - i > 5)) break;
				//if (tasks[i].EndTime < frame.startTime) break;
				else start_at = i;
			}

			float test55 = ((long long)(tasks[start_at].StartTime - frame.startTime)) / 1000.0f;
			float frame_pixel_start = (long long)(frame.startTime - tasks[start_at].StartTime) / 1000.0f * 100.f * ms_size;

			ImVec2 screenPos = ImGui::GetCursorScreenPos();


			auto drawList = ImGui::GetWindowDrawList();

			auto scroll_x = ImGui::GetScrollX();

			float a = 5;
			float b = 25;

			std::remainderf(a, b);

			float start;


			if (scroll_x > frame_pixel_start)
			{
				float distance = scroll_x - frame_pixel_start;
				start = scroll_x - std::remainderf(distance, 100.f * ms_size);
			}
			else
			{
				float distance = frame_pixel_start - scroll_x;
				auto test = std::remainderf(distance, 10.f * ms_size);

				start = std::remainderf(distance, 10.f * ms_size) + scroll_x;
			}

			auto max_tmp = scroll_x + windowSize.x;

			std::cout << "windowSize: " << windowSize.x << std::endl;

			for (float max = scroll_x + windowSize.x; start < max; start += 10.f * ms_size)
			{

				int time = std::roundf(start - frame_pixel_start);
				if (time < 0) time *= -1;
				time /= 10.f * ms_size;

				if (time % 10 == 0)
				{
					drawList->AddLine(ImVec2(screenPos.x + start, screenPos.y + 50), ImVec2(screenPos.x + start, screenPos.y + 400), IM_COL32(255, 255, 255, 255), 1.0f);

					std::string text = std::to_string((int)(time * .1f)) + "ms";
					ImVec2 tSize = ImGui::CalcTextSize(text.c_str());

					drawList->AddText(ImVec2(screenPos.x + start + 5 - tSize.x * .5f, screenPos.y + 30), IM_COL32(255, 255, 255, 255), text.c_str());
				}
				else
				{
					drawList->AddLine(ImVec2(screenPos.x + start, screenPos.y + 55), ImVec2(screenPos.x + start, screenPos.y + 400), IM_COL32(120, 120, 120, 255), 1.0f);

					
					if (ms_size >= 5.0f)
					{
						std::string nmbr = std::to_string((int)(time * 10.f));
						std::u8string text =  std::u8string(nmbr.begin(), nmbr.end()) + std::u8string(u8"\u00B5s");
						
						ImVec2 tSize = ImGui::CalcTextSize((const char*)text.c_str());
						drawList->AddText(ImVec2(screenPos.x + start + 5 - tSize.x * .5f, screenPos.y + 30), IM_COL32(120, 120, 120, 255), (const char*)text.c_str());
					}
				}
			}

			drawList->AddLine(ImVec2(screenPos.x + frame_pixel_start, screenPos.y + 10), ImVec2(screenPos.x + frame_pixel_start, screenPos.y + 20), IM_COL32(255, 255, 255, 255), 2.0f);
			drawList->AddLine(ImVec2(screenPos.x + frame_pixel_start + frame.frameTime / 1000.0f * 100.f * ms_size, screenPos.y + 10), ImVec2(screenPos.x + frame_pixel_start + frame.frameTime / 1000.0f * 100.f * ms_size, screenPos.y + 20), IM_COL32(255, 255, 255, 255), 2.0f);

			drawList->AddLine(ImVec2(screenPos.x + frame_pixel_start, screenPos.y + 15), ImVec2(screenPos.x + frame_pixel_start + frame.frameTime / 1000.0f * 100.f * ms_size, screenPos.y + 15), IM_COL32(255, 255, 255, 255), 2.0f);



			for (int i = start_at; i < tasks.size(); i++)
			{
				if (tasks[i].StartTime > frame.startTime + frame.frameTime)
				{
					long long diff = (tasks[i].StartTime - frame.startTime);
					ImVec2 pos = ImVec2((diff / 1000.0f * 100.f) * ms_size + frame_pixel_start,  60);
					ImGui::SetCursorPos(pos);
					ImGui::InvisibleButton("##extend", ImVec2(10, 10));
					break;
				}

				ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(tasks[i].Name[0] * 200 % 255, tasks[i].Name[1] * 200 % 255, tasks[i].Name[2] * 200 % 255, 255));
				//ImGui::PushStyleColor(ImGuiCol_Button, i >= start_at ? IM_COL32(255, 255, 255, 255) : IM_COL32(255, 0, 0, 255));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32(255, 255, 255, 255));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, IM_COL32(255, 255, 255, 255));
				float brightness = 0.299f * tasks[i].Name[0] + 0.587f * tasks[i].Name[1] + 0.114f * tasks[i].Name[2];
				ImGui::PushStyleColor(ImGuiCol_Text, brightness > 127 ? IM_COL32(0, 0, 0, 255) : IM_COL32(255, 255, 255, 255));
				//ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 0, 0, 255));

				long long diff = (tasks[i].StartTime - frame.startTime);
				float start = diff / 1000.0f;
				ImVec2 pos = ImVec2((diff / 1000.0f * 100.f) * ms_size + frame_pixel_start, tasks[i].Depth * 30 + 60);
				//ImVec2 pos = ImVec2((diff / 1000.0f * 100.f - half_width) * ms_size + cursor_pos_offset, tasks[i].Depth * 30 + 50);

				ImGui::SetCursorPos(pos);
				ImGui::Button(tasks[i].Name.c_str(), ImVec2((tasks[i].EndTime - tasks[i].StartTime) / 1000.0f * 100.f * ms_size, 20));

				if (ImGui::IsItemHovered())
				{
					ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 255));
					ImGui::BeginTooltip();
					ImGui::Text("Name: %s", tasks[i].Name.c_str());
					ImGui::Text("Start: %.0f", tasks[i].StartTime / 1000.0f);
					ImGui::Text("End: %.0f", tasks[i].EndTime / 1000.0f);
					ImGui::Text("Duration: %.4fms", (tasks[i].EndTime - tasks[i].StartTime) / 1000.0f);
					ImGui::EndTooltip();
					ImGui::PopStyleColor();
				}

				ImGui::PopStyleColor(4);
			}

			ImGui::EndChild();
			ImGui::PopStyleColor();
		}

		void OnUpdate() override
		{
			if (ImGui::Begin("Profiler", &this->m_window_open))
			{
				HL_PROFILE_BEGIN("GUI - Profiler");
				bool isProfilingThisFrame = Profiler::isProfiling();

				ImGui::Text("FPS: %f", Time::FramesPerSecond());

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

				if (ImGui::Button("Clear")) { Profiler::clear(); m_inspecting_frame = 0u; }

				if (Profiler::isProfiling())
				{
					if (slow_mode)
					{
						static float tmr = update_interval;
						tmr += Time::DeltaTime();

						if (tmr >= update_interval)
						{
							if (Profiler::getFrameResults().size() > 1) m_inspecting_frame = Profiler::getFrameResults().size() - 2;
							tmr = 0.0f;
						}
					}
					else if (Profiler::getFrameResults().size() > 1) m_inspecting_frame = Profiler::getFrameResults().size() - 2;
				}

				auto size = Profiler::getFrameResults().size();

				if (Profiler::getFrameResults().size() > 0)
				{
					auto test = Profiler::getFrameResults();
					auto frameResult = Profiler::getFrameResults().back();

					if (frameResult.frameTime)
					{
						uint32_t selectedFrame = 0u;
						unsigned long long frametime = frameResult.frameTime / 1000.0f;

						ImGui::Text((std::to_string(Profiler::getFrameResults().size() - 1) + " Frames!").c_str());
						ImGui::Text(("Last frame time: " + std::to_string(frameResult.frameTime / 1000.0f) + "ms.").c_str());

						ImGui::PlotLines("Frametime", [](void* data, int idx) { return (Profiler::getFrameResults()[std::min(std::max<uint64_t>(Profiler::getFrameResults().size() - 300, 0ull) + idx, Profiler::getFrameResults().size() - 1)].frameTime / 1000.0f); }, nullptr, std::min(Profiler::getFrameResults().size() - 1, 299ull), 0, 0, FLT_MAX, FLT_MAX, ImVec2(1800, 150));


						ImGui::Checkbox("Slow Mode", &slow_mode);
						ImGui::SameLine();
						ImGui::DragFloat("Update Interval", &update_interval);

						DrawTimline();

						/*HL_PROFILE_BEGIN("GUI - Profiler DRAW");
						ProfilerTimeline::Draw(selectedFrameProfile);
						HL_PROFILE_END();*/

						/*for (auto x : selectedFrameProfile.results)
						{
							ImGui::Button(x.Name, ImVec2(0, 0));
						}*/
					}
				}

				HL_PROFILE_END();
			}
			ImGui::End();
		}
	};
}
