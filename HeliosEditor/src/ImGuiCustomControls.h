#pragma once

#include <Helios/Translation/Vector.h>
#include <imgui.h>

namespace ImGui {
	
	void EditVector3(const char* label, Helios::Vector3& vec, float speed = 0.1f, float min = 0.0f, float max = 0.0f) {

		ImVec2 size = ImGui::GetContentRegionAvail();

		ImGui::PushID(label);
		ImGui::Text(label);
		ImGui::SameLine();
		ImGui::SetCursorPosX(size.x / 2.5f - 20.0f);
		ImGui::Text("X:");
		ImGui::SameLine();
	
		ImGui::SetNextItemWidth(size.x / 7.0f);
		ImGui::DragFloat((std::string("##x") + label).c_str(), &vec[0], speed, min, max);
		ImGui::SameLine();
		ImGui::Text("Y:");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(size.x / 7.0f);
		ImGui::DragFloat((std::string("##y") + label).c_str(), &vec[1], speed, min, max);
		ImGui::SameLine();
		ImGui::Text("Z:");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(size.x / 7.0f);
		ImGui::DragFloat((std::string("##z") + label).c_str(), &vec[2], speed, min, max);
		ImGui::PopID();
	}

	void EditQuanterionEuler(const char* label, Helios::Quanterion& quanterion, float speed = 0.1f, float min = 0.0f, float max = 0.0f) {

		ImVec2 size = ImGui::GetContentRegionAvail();
		static Helios::Vector3 euler = quanterion.euler();
		bool is_changed = false;

		ImGui::PushID(label);
		ImGui::Text(label);
		ImGui::SameLine();
		ImGui::SetCursorPosX(size.x / 2.5f - 20.0f);
		ImGui::Text("X:");
		ImGui::SameLine();

		ImGui::SetNextItemWidth(size.x / 7.0f);
		if (ImGui::DragFloat((std::string("##x") + label).c_str(), &euler.x, speed, min, max)) is_changed = true;
		ImGui::SameLine();
		ImGui::Text("Y:");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(size.x / 7.0f);
		if (ImGui::DragFloat((std::string("##y") + label).c_str(), &euler.y, speed, min, max)) is_changed = true;
		ImGui::SameLine();
		ImGui::Text("Z:");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(size.x / 7.0f);
		if (ImGui::DragFloat((std::string("##z") + label).c_str(), &euler.z, speed, min, max)) is_changed = true;
		ImGui::PopID();
		if (is_changed) quanterion = Helios::Quanterion::FromEuler(euler);
	}

	void EditQuanterion(const char* label, Helios::Quanterion& quanterion, float speed = 0.1f, float min = 0.0f, float max = 0.0f) {

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
}