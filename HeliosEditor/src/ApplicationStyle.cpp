#pragma once
#include "ApplicationStyle.h"

#include "Icons.h"
#include "pch.h"

#define COL32A(R,G,B,A) ImVec4((R)/255.0f, (G)/255.0f, (B)/255.0f, (A)/255.0f)
#define COL32(R,G,B) COL32A(R,G,B,255)

namespace Helios
{
	void InitImGuiStyle()
	{
		ImGuiIO& io = ImGui::GetIO();
		//io.Fonts->AddFontDefault();

		ImFontConfig icons_config;
		icons_config.MergeMode = true;

		static const ImWchar icons_ranges[] = { ICONS_FONT_MIN, ICONS_FONT_MAX, 0 };

		io.FontDefault = io.Fonts->AddFontFromFileTTF("fonts/NotoSans/NotoSans-Regular.ttf", 17.0f, nullptr, io.Fonts->GetGlyphRangesCyrillic());
		io.Fonts->AddFontFromFileTTF("fonts/NotoSans JP/static/NotoSansJP-Regular.ttf", 17.0f, &icons_config, io.Fonts->GetGlyphRangesJapanese());
		io.Fonts->AddFontFromFileTTF("fonts/NotoSans SC/static/NotoSansSC-Regular.ttf", 17.0f, &icons_config, io.Fonts->GetGlyphRangesChineseFull());
		io.Fonts->AddFontFromFileTTF("fonts/icons.ttf", 17.0f, &icons_config, icons_ranges);


		io.Fonts->AddFontFromFileTTF("fonts/NotoSans/NotoSans-Bold.ttf", 17.0f, nullptr, io.Fonts->GetGlyphRangesCyrillic());		
		io.Fonts->AddFontFromFileTTF("fonts/icons.ttf", 17.0f, &icons_config, icons_ranges);
		

		auto add_col = [](ImVec4 col, float offset) {
			return ImVec4{ col.x + offset, col.y + offset, col.z + offset, 1.0f };
		};

		auto& colors = ImGui::GetStyle().Colors;
		colors[ImGuiCol_WindowBg] = ImVec4(37.0f / 255.0f,36.0f / 255.0f,40.0f / 255.0f, 1.0f);
		
		// Headers
		colors[ImGuiCol_Header] = { 0.141f, 0.149f, 0.145f, 1.0f };
		colors[ImGuiCol_HeaderHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
		colors[ImGuiCol_HeaderActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

		// Buttons
		colors[ImGuiCol_Button] = { 0.078f, 0.078f, 0.078f, 1.0f };
		colors[ImGuiCol_ButtonHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
		colors[ImGuiCol_ButtonActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

		// Frame BG
		colors[ImGuiCol_FrameBg] = { 0.1f, 0.12f, 0.12f, 1.0f };
		colors[ImGuiCol_FrameBgHovered] = add_col(colors[ImGuiCol_FrameBg], 0.1f);
		colors[ImGuiCol_FrameBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

		// Tabs
		colors[ImGuiCol_Tab] = add_col(colors[ImGuiCol_WindowBg], 0.1f);
		colors[ImGuiCol_TabHovered] = ImVec4{ 0.38f, 0.3805f, 0.381f, 1.0f };
		colors[ImGuiCol_TabActive] = COL32(244, 35, 0);
		colors[ImGuiCol_TabUnfocused] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
		colors[ImGuiCol_TabUnfocusedActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

		colors[ImGuiCol_Separator] = ImVec4{ 0.2f, 0.22f, 0.22f, 1.0f };
		
		// Title
		colors[ImGuiCol_TitleBg] = { 0.105f, 0.105f, 0.109f, 1.0f };
		colors[ImGuiCol_TitleBgActive] = { 0.212f, 0.208f, 0.220f, 1.0f };
		colors[ImGuiCol_TitleBgCollapsed] = { 0.212f, 0.208f, 0.220f, 1.0f };
	}

}
