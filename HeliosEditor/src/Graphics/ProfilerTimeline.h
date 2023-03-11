#pragma once

#include "pch.h"
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
			float zoomReverse = zoom / 1000.0f;

			ImGui::DragFloat("Zoom", &zoom);

			ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(15, 15, 15, 255));
			ImGui::BeginChild("timeline", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

			uint32_t timelineMaxRange = std::ceil(profile.frameTime / 1000.0f / 5.0f) * 5;

			static float timelineOffset = 300;

			static uint32_t timezoom = 1u;

			//timezoom += max(min(ImGui::GetScrollY(), 5us_MouseWheelDelta), 1u);

			ImVec2 screenPos = ImGui::GetCursorScreenPos();

			auto p0 = ImGui::GetItemRectMin();
			auto p1 = ImGui::GetItemRectMax();
			auto drawList = ImGui::GetWindowDrawList();
			//drawList->PushClipRect(p0, p1);
			for (uint32_t i = 0; i <= timelineMaxRange; i = i + timezoom)
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
			}
			//drawList->PopClipRect();

			uint32_t depth = 0u;
			unsigned long long timelines[1000] = { 0u };

			uint32_t i = 0;
			ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(128, 128, 128, 255));

			for (auto& result : profile.results)
			{
				if (result.StartTime > timelines[depth])
				{
					uint32_t tmp;
					if(result.StartTime > timelines[tmp = min(depth - 1, 0u)])
					{
						depth = tmp;
					}
					//depth = min(depth - 1, 0u);
				}
				else
					depth++;

				timelines[depth] = result.EndTime;

				ImGui::SetCursorScreenPos(ImVec2(timelineOffset + screenPos.x + (result.StartTime - profile.startTime) * zoomReverse, screenPos.y + 50 * depth + 25));
				ImGui::Button((result.Name + std::string("#") + result.ThreadName).c_str(), ImVec2((result.EndTime - result.StartTime) * zoomReverse, 35));
				if(ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
				{
					ImGui::SetTooltip((result.Name + std::string(" : ") + std::to_string((result.EndTime - result.StartTime) / 1000.0f) + "ms.").c_str());
				}
				i++;
			}
			ImGui::PopStyleColor();


			//ImGui::Text(std::to_string(InputManager::GetMouseWheelDelta()).c_str());

			ImGui::EndChild();
			ImGui::PopStyleColor();
		}
	};
}

