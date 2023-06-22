#include "pch.h"
#include "ImGuiCustomControls.h"

#include "imgui_internal.h"

namespace Helios
{
	
}

bool ImGui::EditVector3(const char* label, float width, Helios::Vector3& vec, float speed, float min, float max)
{
	bool edited = false;

	ImGui::PushID(label);

	
	ImGui::PushStyleVar( ImGuiStyleVar_ItemSpacing, ImVec2(0, 0) );
	ImGui::BeginChild("##vector3", ImVec2(width, 25.0f), false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBringToFrontOnFocus);
	ImVec2 size = ImGui::GetContentRegionAvail();

	ImGui::Text(label);

	float x = size.x / 3.5f;
	float w = size.x - x;
	float offset = w / 3.0f;
	float input_width = w / 3.0f - size.y;

	ImGui::SetCursorPos(ImVec2(x, 0.0f));

	ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
	ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(255, 0, 0, 255));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32(255, 0, 0, 255));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive,	 IM_COL32(255, 0, 0, 255));
	ImGui::Button("X", ImVec2(size.y, size.y));
	ImGui::PopStyleColor(3);
	ImGui::PopFont();
	ImGui::SameLine();

	ImGui::SetNextItemWidth(input_width);
	if (ImGui::DragFloat((std::string("##x") + label).c_str(), &vec[0], speed, min, max) )
		edited = true;
	ImGui::SameLine();

	ImGui::SetCursorPos(ImVec2(x + offset, 0.0f));

	ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
	ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(0, 0, 255, 255));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32(0, 0, 255, 255));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive,	 IM_COL32(0, 0, 255, 255));
	ImGui::Button("Y", ImVec2(size.y, size.y));
	ImGui::PopStyleColor(3);
	ImGui::PopFont();
	ImGui::SameLine();

	ImGui::SetNextItemWidth(input_width);
	if (ImGui::DragFloat((std::string("##y") + label).c_str(), &vec[1], speed, min, max))
		edited = true;
	ImGui::SameLine();

	ImGui::SetCursorPos(ImVec2(x + offset + offset, 0.0f));

	ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
	ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(0, 255, 0, 255));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32(0, 255, 0, 255));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, IM_COL32(0, 255, 0, 255));
	ImGui::Button("Z", ImVec2(size.y, size.y));
	ImGui::PopStyleColor(3);
	ImGui::PopFont();
	ImGui::SameLine();

	ImGui::SetNextItemWidth(input_width);
	if (ImGui::DragFloat((std::string("##z") + label).c_str(), &vec[2], speed, min, max))
		edited = true;
	ImGui::SameLine();
	
	ImGui::EndChild();
	ImGui::PopStyleVar();

	ImGui::PopID();

	return edited;
}

void ImGui::EditQuanterionEuler(const char* label, float width, Helios::Quaternion& quanterion, float speed, float min, float max)
{
	Helios::Vector3 euler = quanterion.euler();
	if(ImGui::EditVector3(label, width, euler, speed, min, max))
		quanterion = Helios::Quaternion::FromEuler(euler);
}

void ImGui::EditQuanterion(const char* label, Helios::Quaternion& quanterion, float speed, float min, float max)
{

	ImVec2 size = ImGui::GetContentRegionAvail();

	ImGui::PushID(label);
	ImGui::Text(label);
	ImGui::SameLine();
	ImGui::SetCursorPosX(size.x / 4.5f - 20.0f);
	ImGui::Text("X:");
	ImGui::SameLine();

	ImGui::SetNextItemWidth(size.x / 7.0f);
	ImGui::DragFloat((std::string("##x") + label).c_str(), &quanterion.x, speed, min, max);
	ImGui::SameLine();
	ImGui::Text("Y:");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(size.x / 7.0f);
	ImGui::DragFloat((std::string("##y") + label).c_str(), &quanterion.y, speed, min, max);
	ImGui::SameLine();
	ImGui::Text("Z:");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(size.x / 7.0f);
	ImGui::DragFloat((std::string("##z") + label).c_str(), &quanterion.z, speed, min, max);
		
	ImGui::SameLine();
	ImGui::Text("W:");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(size.x / 7.0f);
	ImGui::DragFloat((std::string("##w") + label).c_str(), &quanterion.w, speed, min, max);

	ImGui::PopID();
}
