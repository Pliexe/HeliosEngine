#pragma once

#include "pch.h"
#include "BasePanel.h"
#include <Helios/Scene/SceneRegistry.h>
#include <Helios/Scene/Components/Lighting.h>

namespace Helios
{
	class ShadowMapsPanel : public Editor::IPanel
	{
	public:
		ShadowMapsPanel() { m_title = "Shadow Maps"; }

		void OnUpdate() override
		{
			auto& components = SceneRegistry::GetSceneByIndex(0).lock()->GetComponents();

			auto view = components.view<const TransformComponent, const DirectionalLightComponent>();

			for (auto entity : view)
			{
				auto [trans, light] = view.get<const TransformComponent, const DirectionalLightComponent>(entity);
				// if (light.framebuffer)
				// {
				// 	Quaternion tmp = trans.Rotation;
				// 	ImGui::EditQuanterionEuler("Light", 500, tmp);
				// 	ImGui::Image((void*)light.framebuffer->GetDepthTextureID(), { 512, 512 });
				// 	//ImGui::Image((void*)light.framebuffer->GetDepthTextureID(), { 256, 256 });
				// }

			}
		}
	};
}