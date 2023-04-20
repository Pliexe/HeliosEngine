#pragma once

#include <imgui.h>

#include "Helios/Core/Application.h"
#include "Helios/Core/Profiler.h"
#include "Helios/Input/InputManager.h"

namespace Helios
{
	class ProfilerTimeline
	{
	public:
		static void Draw(Profiler::FrameProfile& profile)
		{
			static float zoom = 100.0f;
			float zoomReverse = zoom * 0.001;
			static float scroll_max_x_prev = ImGui::GetScrollMaxX();

			if(ImGui::DragFloat("Zoom", &zoom))
			{
				float scroll_max_x_total = scroll_max_x_prev + ImGui::GetWindowWidth();
				float mouseX = ImGui::GetMousePos().x - ImGui::GetWindowPos().x;
				float scroll_x_percent = (ImGui::GetScrollX() + mouseX) / scroll_max_x_total;
				ImGui::SetScrollX(scroll_max_x_total * scroll_x_percent);
			}

			ImGui::Text(std::to_string(InputManager::GetMouseWheelDelta()).c_str());

			ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(15, 15, 15, 255));
			ImGui::BeginChild("timeline", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

			uint32_t timelineMaxRange = std::ceil(profile.frameTime / 1000.0f / 5.0f) * 5; // / 5 0> * 0.2 | / 1000 -> * 0.001

			static float timelineOffset = 0;
			//static float timelineOffset = 300;


			ImGuiIO& io = ImGui::GetIO();
			bool is_scrolling = ImGui::IsWindowHovered() && io.MouseWheel != 0.0f;

			if (is_scrolling)
				zoom += (io.KeyShift ? 100.0f : 40.0f) * io.MouseWheel;


			static uint32_t timezoom = 1u;

			//timezoom += max(min(ImGui::GetScrollY(), 5us_MouseWheelDelta), 1u);

			ImVec2 screenPos = ImGui::GetCursorScreenPos();
			
			auto drawList = ImGui::GetWindowDrawList();
			//drawList->PushClipRect(p0, p1);
			HL_PROFILE_BEGIN("ProfilerTimeline - Time");
			/*for (uint32_t i = 0; i <= timelineMaxRange; i = i + timezoom)
			{
				if(zoom > 10.0f)
				{
					ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(100, 100, 100, 255));
					for(uint32_t j = 1; j < 10; j++)
					{
						auto tmp = std::to_string(j * 100) + std::string(u8"µs");
						auto textWidth = ImGui::CalcTextSize(tmp.c_str()).x;

						drawList->AddLine(ImVec2(timelineOffset + screenPos.x + i * zoom + ((j * (zoom / 10.0f))), screenPos.y + 20), ImVec2(timelineOffset + screenPos.x + i * zoom + ((j * (zoom / 10.0f))), screenPos.y + ImGui::GetWindowHeight()), IM_COL32(100, 100, 100, 255));
						ImGui::SetCursorScreenPos(ImVec2(timelineOffset + screenPos.x + i * zoom + ((j * (zoom / 10.0f))) - (textWidth * 0.5f), screenPos.y));
						ImGui::Text(tmp.c_str());
						ImGui::SameLine();
					}
					ImGui::PopStyleColor();
				}
				drawList->AddLine(ImVec2(timelineOffset + screenPos.x + i * zoom, screenPos.y + 20), ImVec2(timelineOffset + screenPos.x + i * zoom, screenPos.y + ImGui::GetWindowHeight()), IM_COL32(230, 230, 230, 255));
				auto tmp = std::to_string(i) + "''";
				auto textWidth = ImGui::CalcTextSize(tmp.c_str()).x;
				ImGui::SetCursorScreenPos(ImVec2(timelineOffset + screenPos.x + i * zoom - (textWidth * 0.5f), screenPos.y));
				ImGui::Text(tmp.c_str());
				ImGui::SameLine();
			}*/

			ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(100, 100, 100, 255));
			float secondTenth = zoom * 0.1f;
			for (uint32_t i = 0, n = timelineMaxRange * 10; i <= n; i++)
			{
				/*if(i % 2)
					drawList->AddLine(ImVec2(timelineOffset + screenPos.x + i * secondTenth, screenPos.y + 30), ImVec2(timelineOffset + screenPos.x + i * secondTenth, screenPos.y + ImGui::GetWindowHeight()), IM_COL32(100, 100, 100, 255));
				else
					drawList->AddLine(ImVec2(timelineOffset + screenPos.x + i * secondTenth, screenPos.y + 20), ImVec2(timelineOffset + screenPos.x + i * secondTenth, screenPos.y + ImGui::GetWindowHeight()), IM_COL32(230, 230, 230, 255));*/

				if(i % 2)
				{
					//auto tmp = std::to_string(i % 10 * 100) + std::string(u8"µs");
					drawList->AddLine(ImVec2(timelineOffset + screenPos.x + i * secondTenth, screenPos.y + 30), ImVec2(timelineOffset + screenPos.x + i * secondTenth, screenPos.y + ImGui::GetWindowHeight()), IM_COL32(100, 100, 100, 255));

					//drawList->AddLine(ImVec2(timelineOffset + screenPos.x + i * zoom + ((i % 10 * (zoom / 10.0f))), screenPos.y + 20), ImVec2(timelineOffset + screenPos.x + i * zoom + ((i % 10 * (zoom / 10.0f))), screenPos.y + ImGui::GetWindowHeight()), IM_COL32(100, 100, 100, 255));
				} else
				{
					ImU32 col = i % 10 ? IM_COL32(150, 150, 150, 255) : IM_COL32(255, 255, 255, 255);

					/*if(i % 10)
						ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(150, 150, 150, 255));
					else
						ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 255));*/

					ImGui::PushStyleColor(ImGuiCol_Text, col);

					drawList->AddLine(ImVec2(timelineOffset + screenPos.x + i * secondTenth, screenPos.y + 20), ImVec2(timelineOffset + screenPos.x + i * secondTenth, screenPos.y + ImGui::GetWindowHeight()), col);
					
					std::string tmp = std::to_string(i * 0.1f);
					for (int n = tmp.size() - 1, i = n; i >= 0; i--)
						if(tmp[i] != '.' && tmp[i] != '0')
						{
							if (i != n) tmp = tmp.substr(0, i+1); else break;
						}
					tmp += "ms";

					//std::string tmp = std::to_string(i * 0.1f) + "ms";
					auto textWidth = ImGui::CalcTextSize(tmp.c_str()).x;
					ImGui::SetCursorScreenPos(ImVec2(timelineOffset + screenPos.x + i * secondTenth - (textWidth * 0.5f), screenPos.y));
					//ImGui::SetCursorScreenPos(ImVec2(timelineOffset + screenPos.x + i * secondTenth, screenPos.y));
					ImGui::Text(tmp.c_str());
					ImGui::PopStyleColor();
				}
				ImGui::SameLine();
			}
			ImGui::PopStyleColor();

			HL_PROFILE_END();

			uint32_t depth = 0u;
			unsigned long long timelines[1000] = { 0u };

			uint32_t i = 0;
			ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(128, 128, 128, 255));

			HL_PROFILE_BEGIN("ProfilerTimeline - Results");
			//for (auto& result : profile.results)
			//{
			//	if (result.StartTime > timelines[depth])
			//	{
			//		uint32_t tmp;
			//		if(result.StartTime > timelines[tmp = min(depth - 1, 0u)])
			//		{
			//			depth = tmp;
			//		}
			//		//depth = min(depth - 1, 0u);
			//	}
			//	else
			//		depth++;

			//	timelines[depth] = result.EndTime;

			//	ImGui::SetCursorScreenPos(ImVec2(timelineOffset + screenPos.x + (result.StartTime - profile.startTime) * zoomReverse, screenPos.y + 50 * depth + 40));
			//	ImGui::Button((result.Name + std::string("#") + result.ThreadName).c_str(), ImVec2((result.EndTime - result.StartTime) * zoomReverse, 35));
			//	if(ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
			//	{
			//		ImGui::SetTooltip((result.Name + std::string(" : ") + std::to_string((result.EndTime - result.StartTime) / 1000.0f) + "ms.").c_str());
			//	}
			//	i++;
			//}

			//ImGui::PushStyleVar(ImGuiStyleVar_)
			for (auto& result : profile.results)
			{
				if (depth == 0 && result.StartTime <= timelines[depth])
					depth++;
				else
				{
					for (int i = 0; i < depth; i++)
					{
						if (result.StartTime > timelines[i])
						{
							depth = i;
							break;
						}
						else if (i == depth - 1) depth++;
					}
				}
				
				timelines[depth] = result.EndTime;

				ImGui::SetCursorScreenPos(ImVec2(timelineOffset + screenPos.x + (result.StartTime - profile.startTime) * zoomReverse, screenPos.y + 50 * depth + 40));
				ImGui::Button((result.Name).c_str(), ImVec2((result.EndTime - result.StartTime) * zoomReverse, 35));

				/*float x1 = timelineOffset + screenPos.x + (result.StartTime - profile.startTime) * zoomReverse;
				float y1 = screenPos.y + 50 * depth + 40;
				float x2 = (result.EndTime - result.StartTime) * zoomReverse;
				float x22 = x2 + x1;

				drawList->AddQuadFilled(ImVec2(x1, y1), ImVec2(x22, y1), ImVec2(x22, y1 + 35), ImVec2(x1, y1 + 35), IM_COL32(100, 100, 100, 255));
				drawList->AddText(ImVec2(x1 + x2 * 0.5f, y1), IM_COL32(255, 255, 255, 255), (result.Name + std::string("#") + result.ThreadName).c_str());*/


			    if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
				{
					ImGui::SetTooltip((result.Name + std::string(" : ") + std::to_string((result.EndTime - result.StartTime) / 1000.0f) + "ms.").c_str());
				}
				i++;
			}
			ImGui::PopStyleColor();
			HL_PROFILE_END();

			HL_PROFILE_BEGIN("Profiler - Scroll Calculation");
			HL_PROFILE_BEGIN("Profiler - Scroll Calculation : scroll_max_x_total");
			float scroll_max_x_total = scroll_max_x_prev + ImGui::GetWindowWidth();
			HL_PROFILE_END();
			HL_PROFILE_BEGIN("Profiler - Scroll Calculation : mouseX");
			float mouseX = ImGui::GetMousePos().x - ImGui::GetWindowPos().x;
			HL_PROFILE_END();
			HL_PROFILE_BEGIN("Profiler - Scroll Calculation : scroll_x_percent");
			float scroll_x_percent = (ImGui::GetScrollX() + mouseX) / scroll_max_x_total;
			HL_PROFILE_END();

			HL_PROFILE_BEGIN("Profiler - Scroll Calculation : is scrolling?");
			if(is_scrolling)
				ImGui::SetScrollX(scroll_max_x_total * scroll_x_percent - mouseX);
			HL_PROFILE_END();

			HL_PROFILE_BEGIN("Profiler - Scroll Calculation : apply prev");
			scroll_max_x_prev = ImGui::GetScrollMaxX();
			HL_PROFILE_END();
			HL_PROFILE_END();

			ImGui::EndChild();
			ImGui::PopStyleColor();
		}
	};
}

