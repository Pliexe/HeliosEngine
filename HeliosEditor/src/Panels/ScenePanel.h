#pragma once

#include <Helios/Graphics/Framebuffer.h>
#include <Helios/Scene/EditorCamera.h>
#include <Helios/Scene/Scene.h>
#include <Helios/Graphics/GizmosRenderer.h>

#include "BasePanel.h"


namespace Helios
{

	struct Line
	{
		Helios::Vector3 a;
		Helios::Vector3 b;
		Helios::Color color = Helios::Color::White;
		float width = 0.5f;
		int64_t id = -1;
		Helios::GizmosRenderer::LineMode mode = Helios::GizmosRenderer::LineMode::Solid;
	};

	class ScenePanel : public Editor::IPanel
	{
		Ref<Framebuffer> m_Framebuffer;

		EditorCamera m_EditorCamera;
		EditorCamera m_EditorCameraSwap;
		bool showIdFrame = false;
		bool show_gizmos = false;
		bool m_isVisible  = true;

		GizmosRenderer::Tool current_tool = GizmosRenderer::Tool::None;

		std::queue<Line> lines;
		std::queue<Line> tool_lines;

		inline static std::vector<ScenePanel*> m_ScenePanels;

	public:
		
		ScenePanel()
		{
			title = "Scene";
			m_Framebuffer = Framebuffer::Create(300, 300, {
				Framebuffer::Format::R32G32B32A32F,
				Framebuffer::Format::R32F,
				Framebuffer::Format::D32F,
			});
			assert(m_Framebuffer != nullptr);
			m_ScenePanels.push_back(this);
			m_custom_begin = true;
		}
		~ScenePanel() { m_ScenePanels.erase(std::find(m_ScenePanels.begin(), m_ScenePanels.end(), this)); }

		static void Render(Scene& scene);

		void ResetControls();

		void OnUpdate() override;
	};
}
