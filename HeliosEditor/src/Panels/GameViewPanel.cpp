#include "GameViewPanel.h"

#include "Application.h"
#include "imgui.h"

namespace Helios
{
	void GameViewPanel::OnUpdate()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		if (ImGui::Begin("Game", 0, flags))
		{
			if (ImGui::BeginMenuBar())
			{
				ImGui::EndMenuBar();
			}

			if (static_cast<HeliosEditor&>(HeliosEditor::GetInstance()).gameFrame != nullptr) {
				ImVec2 viewportSize = ImGui::GetContentRegionAvail();
				auto currentViewportSize = static_cast<HeliosEditor&>(HeliosEditor::GetInstance()).gameFrame->GetSize();
				if (viewportSize.x != currentViewportSize.width || viewportSize.y != currentViewportSize.height)
				{
					static_cast<HeliosEditor&>(HeliosEditor::GetInstance()).gameFrame->Resize(viewportSize.x, viewportSize.y);
				}
				ImGui::Image(static_cast<HeliosEditor&>(HeliosEditor::GetInstance()).gameFrame->GetTextureID(0u), ImVec2(viewportSize));
			}

			static_cast<HeliosEditor&>(HeliosEditor::GetInstance()).isGameSceneActive = true;
		}
		else static_cast<HeliosEditor&>(HeliosEditor::GetInstance()).isGameSceneActive = false;
		ImGui::End();
		ImGui::PopStyleVar();
	}
}
