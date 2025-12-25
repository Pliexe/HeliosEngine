#pragma once

#include <Helios/Scene/EditorCamera.h>
#include <Helios/Scene/Scene.h>
#include <Helios/Graphics/GPURenderPass.h>

#include "BasePanel.h"
#include "Helios/Core/Application.h"
#include "Helios/Graphics/RenderGraph/RenderContext.h"
#include "Helios/Graphics/RenderGraph/RenderPasses/Forward.h"
#include "Helios/Resources/Buffer.h"
#include "Helios/Resources/Image.h"
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
		// Helios::GizmosRenderer::LineMode mode = Helios::GizmosRenderer::LineMode::Solid;
	};

	class ScenePanel : public Editor::IPanel
	{
	private:
		// Ref<Framebuffer> m_Framebuffer;

		Ref<Image> m_colorImage;

		EditorCamera m_EditorCamera;
		EditorCamera m_EditorCameraSwap;
		bool showIdFrame = false;
		bool showDepth = false;
		bool show_vertecies = false;
		bool m_isVisible  = true;

		// GizmosRenderer::ToolType active_tool_type = GizmosRenderer::ToolType::Grab;

		inline static std::vector<ScenePanel*> m_ScenePanels;

		struct ToolData
		{
			// GizmosRenderer::Tool tool = GizmosRenderer::Tool::None;
			TransformComponent world_transform;
			TransformComponent local_transform;
			Entity entity;
		} tool_data;

		struct Vert
		{
			Vector3 pos;
		};

		Ref<VertexBuffer<Vert>> m_vertexBuffer;
		Ref<IndexBuffer> m_indexBuffer;

	public:
		
		ScenePanel()
		{
			m_title = "Scene";

			m_colorImage = Graphics::GetMainDevice()->CreateImage(Image::Usage::ColorAttachment | Image::Usage::Sampled | Image::Usage::TransferDst, Image::Format::R8G8B8A8_UNORM, 300, 300);
			

			Graphics::GetMainRenderGraph()->AddPass("ForwardPass", ForwardPass());
			GizmoPass();
			Graphics::GetMainRenderGraph()->AddPass("Scene", [&](RenderContext& ctx){

				if (ctx.HasList<Ref<Image>>("ForwardPassImage"))
				{
					auto& list = ctx.GetList<Ref<Image>>("ForwardPassImage");
					if (!list.empty()) m_colorImage = list[0];
				}
			});

			Vert vertices[] = {
				{ { -0.5f, -0.5f, 0.0f } },
				{ {  0.5f, -0.5f, 0.0f } },
				{ {  0.5f,  0.5f, 0.0f } },
				{ { -0.5f,  0.5f, 0.0f } },
			};

			uint indices[] = {
				0, 2, 1,
				2, 0, 3,
			};


			m_vertexBuffer = VertexBuffer<Vert>::Create(vertices, 4);
			m_indexBuffer = IndexBuffer::Create(indices, 6);

			// Application::GetInstance().GetWindow()->GetContext().UseContext();
			// m_Framebuffer = Framebuffer::Create(300, 300, {
			// 	Framebuffer::Format::R8G8B8A8_UNORM,
			// 	Framebuffer::Format::R32G32B32A32_INT,
			// 	Framebuffer::Format::D32F,
			// });
			assert(m_colorImage != nullptr);
			// m_RenderPass = GPURenderPass::Create({
			// 	.frameBuffer = m_Framebuffer,
			// 	.attachmentSpecifications = {
			// 		{
			// 			.clearColor = Color::Green
			// 		}
			// 	}
			// });
			m_ScenePanels.push_back(this);
			m_custom_begin = true;
			m_EditorCamera.SetPosition(Vector3(0.0f, 0.0f, -10.0f));
		}    
		~ScenePanel() { m_ScenePanels.erase(std::find(m_ScenePanels.begin(), m_ScenePanels.end(), this)); }
		
		void GizmoPass();
		
		void RenderFramebuffer(NativeCommandList& cmdList);

		void ResetControls();
		// bool ToggleTool(GizmosRenderer::Tool tool);
		void HandleTool(float x, float y, bool start);

		void OnUpdate() override;
	};
}
