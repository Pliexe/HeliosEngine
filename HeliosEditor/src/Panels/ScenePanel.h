#pragma once

#include <Helios/Graphics/Framebuffer.h>
#include <Helios/Scene/EditorCamera.h>
#include <Helios/Scene/Scene.h>
#include <Helios/Graphics/GizmosRenderer.h>

#include "BasePanel.h"
#include "Helios/Core/Application.h"
#include "Helios/Scene/Entity.h"


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
	private:
		Ref<Framebuffer> m_Framebuffer;

		EditorCamera m_EditorCamera;
		EditorCamera m_EditorCameraSwap;
		bool showIdFrame = false;
		bool show_vertecies = false;
		bool m_isVisible  = true;

		GizmosRenderer::ToolType active_tool_type = GizmosRenderer::ToolType::Grab;

		inline static std::vector<ScenePanel*> m_ScenePanels;

		struct ToolData
		{
			GizmosRenderer::Tool tool = GizmosRenderer::Tool::None;
			TransformComponent world_transform;
			TransformComponent local_transform;
			Entity entity;
		} tool_data;

	public:
		
		ScenePanel()
		{
			title = "Scene";
			Application::GetInstance().GetWindow()->GetContext().UseContext();
			m_Framebuffer = Framebuffer::Create(300, 300, {
				Framebuffer::Format::R8G8B8A8_UNORM,
				Framebuffer::Format::R32G32B32A32_INT,
				Framebuffer::Format::D32F,
			});
			assert(m_Framebuffer != nullptr);
			m_ScenePanels.push_back(this);
			m_custom_begin = true;
			m_EditorCamera.SetPosition(Vector3(0.0f, 0.0f, -10.0f));
		}    
		~ScenePanel() { m_ScenePanels.erase(std::find(m_ScenePanels.begin(), m_ScenePanels.end(), this)); }
		
		void RenderFramebuffer();

		void ResetControls();
		bool ToggleTool(GizmosRenderer::Tool tool);
		void HandleTool(float x, float y, bool start);

		void OnUpdate() override;
	};
}
