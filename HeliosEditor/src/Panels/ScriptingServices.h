#pragma once

#include "pch.h"
#include <imgui_internal.h>
#include "BasePanel.h"

#include "Icons.h"

#include <Helios/Scripting/ScriptManager.h>

namespace Helios
{
    class ScriptingServicesPanel : public Editor::IPanel
    {
    public:
        ScriptingServicesPanel()
        {
            m_title = "Scripting Services";
            m_window_open = false;
            m_flags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_Modal;
        }

        void OnUpdate() override
        {
            ImGui::Text("Currently Registered Services:");
            ImGui::Separator();

            auto &services = Helios::Scripting::GetRegisteredServices();

            for (auto &service_name : services)
            {
                auto &service = Helios::Scripting::GetService(service_name);

                ImGui::PushID(service_name.c_str());

                switch (service.GetStatus())
                {
                case Scripting::ServiceStatus::Running:
                    ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), (const char *)ICON_UI_CHECKMARK);
                    break;
                case Scripting::ServiceStatus::Stopped:
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
                    ImGui::Text((const char *)ICON_UI_XMARK);
                    ImGui::PopStyleColor();
                    break;
                case Scripting::ServiceStatus::NotFound:
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.5f, 0.0f, 1.0f));
                    ImGui::Text((const char *)ICON_UI_QUESTIONMARK);
                    ImGui::PopStyleColor();
                    break;
                case Scripting::ServiceStatus::NotInitialized:
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.5f, 0.0f, 1.0f));
                    ImGui::Text((const char *)ICON_UI_EXCLAMATIONMARK);
                    ImGui::PopStyleColor();
                    break;
                }

                ImGui::SameLine();
                ImGui::Text("%s", service_name.c_str());

                bool *p_open = ImGui::GetStateStorage()->GetBoolRef(ImGui::GetID(service_name.c_str()), false);

                ImGuiStyle &style = ImGui::GetStyle();
                ImVec2 arrow_pos = ImVec2(ImGui::GetItemRectMax().x + 25.0f - style.FramePadding.x - ImGui::GetFontSize(), ImGui::GetItemRectMin().y + style.FramePadding.y);
                ImGui::RenderArrow(ImGui::GetWindowDrawList(), arrow_pos, ImGui::GetColorU32(ImGuiCol_Text), *p_open ? ImGuiDir_Down : ImGuiDir_Right);

                if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
                {
                    *p_open ^= 1;
                }

                ImGui::PopID();

                if (*p_open)
                {
                    ImGui::Text("%s: %s", service.GetName().c_str(), to_string(service.GetStatus()));

                    if (auto str = "Behaviours " + std::to_string(service.GetBehaviourNames().size()); ImGui::CollapsingHeader(str.c_str()))
                    {
                        for (auto &behaviour_name : service.GetBehaviourNames())
                        {
                            ImGui::Text("%s", behaviour_name.c_str());
                        }
                    }

                    ImGui::PushStyleColor(ImGuiCol_Button, service.GetStatus() == Scripting::ServiceStatus::Running ? ImVec4(0.6f, 0.0f, 0.0f, 1.0f) : ImVec4(0.0f, 0.6f, 0.0f, 1.0f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, service.GetStatus() == Scripting::ServiceStatus::Running ? ImVec4(0.8f, 0.0f, 0.0f, 1.0f) : ImVec4(0.0f, 0.8f, 0.0f, 1.0f));
                    if (ImGui::Button(service.GetStatus() == Scripting::ServiceStatus::Running ? "Stop" : "Start"))
                    {
                        if (service.GetStatus() == Scripting::ServiceStatus::Running)
                        {
                            Helios::Scripting::StopService(service.GetName());
                        }
                        else
                        {
                            Helios::Scripting::StartService(service.GetName());
                        }
                    }
                    ImGui::PopStyleColor(2);
                }

                ImGui::Separator();
            }
        }
    };
}