#include "ScenePanel.h"

#include "Helios/Core/Profiler.h"
#include "Helios/Graphics/BindingLayout.h"
#include "Helios/Graphics/GPURenderPass.h"
#include "Helios/Graphics/Sampler.h"
#include "Helios/Math/Matrix.h"
#include "Helios/Resources/Buffer.h"
#include "Helios/Resources/ShaderModule.h"
#include "Helios/Scene/EntityHot.h"
#include "imgui.h"
#include "InspectorPanel.h"
#include "Helios/Input/InputManager.h"
#include "Helios/Input/KeyCodes.h"
#include "Application.h"
#include "Helios/Core/Time.h"
#include "Helios/Core/Application.h"
#include <memory>
#include <string>

namespace Helios
{
	void ScenePanel::GizmoPass()
	{
		Graphics::GetMainRenderGraph()->AddPass("VerticiesGizmos", [&](RenderContext& ctx){
			if (InspectorPanel::GetInstance().GetType() != InspectorPanel::SelectedType::Entity) return;

			return;


			if (!ctx.HasList<Ref<Image>>("ForwardPassImage")) return;

			Entity obj = std::any_cast<Entity>(InspectorPanel::GetInstance().GetHandle());

			if (!obj.IsValid()) return;

			if (!obj.HasComponent<MeshRendererComponent>()) return;
			if (!obj.HasComponent<TransformComponent>()) return;

			Transform transform = Transform(obj);

			auto& list = ctx.GetList<Ref<Image>>("ForwardPassImage");
			if (list.empty()) return;
			
			auto& colorImage = list[0];


			static auto il = InputLayouts({
				InputLayout {
					{ "POSITION", ShaderDataType::Float32_3 },
					{ "TEXCOORD", ShaderDataType::Float32_2 },
					{ "NORMAL", ShaderDataType::Float32_3 },
				},
				InputLayout {
					{
						{ "INSTANCEPOS", ShaderDataType::Float32_3 },
						{ "INSTANCESIZE", ShaderDataType::Float32_2 }
					},
					InputClassification::PerInstanceData,
				}
			});
			
			auto cmd = Graphics::GetMainDevice()->GetAllocatorForThread().CreateCommandList();

			cmd->Reset();

			RenderPassBeginInfo beginInfo;
			beginInfo.attachmentSpecifications = {
				{
					.buffer = colorImage,
					.loadOp = RenderPassBeginInfo::LoadOp::Load,
					// .loadOp = RenderPassBeginInfo::LoadOp::Clear,
				}
			};

			cmd->BeginRenderPass(beginInfo);


			static auto shaderModule = CreateRef<ShaderModule>(ShaderModule::ShaderModuleSpecification{
				.family = ShaderModule::Family::Main,
				.stage = ShaderModule::Stage::Vertex | ShaderModule::Stage::Pixel,
				.storageType = ShaderModule::StorageType::Code,
				.path = "Gizmos/VertexGizmo.hsl",
				.inputLayout = il,
			});
			
			struct alignas(16) InfoBuff
			{
				Matrix4x4 view;
				Matrix4x4 proj;
				Vector3 camRight;
				float _pad0;
				Vector3 camUp;
				float _pad1;
				Color color;
			};

			static_assert(sizeof(Matrix4x4) == 64);
			static_assert(sizeof(Color) == 16);
			static_assert(sizeof(InfoBuff) % 16 == 0);

			auto infoBuff = ctx.resources.RequestUnsafeUniformBuffer(sizeof(InfoBuff));
			{
				auto infoBuffView = infoBuff->Lock<InfoBuff>();

				infoBuffView->view = m_EditorCamera.GetViewProjection();
				infoBuffView->proj = Matrix4x4::Identity();
				infoBuffView->camRight = m_EditorCamera.GetRight();
				infoBuffView->camUp = m_EditorCamera.GetUp();
				infoBuffView->_pad0 = 0.f;
				infoBuffView->_pad1 = 0.f;
				infoBuffView->color = Color::Red;

				infoBuff->Unlock();
			}

			struct InstanceBuff
			{
				Vector3 pos;
				Vector2 size;
			};

			auto& mesh = obj.GetComponent<MeshRendererComponent>().mesh;

			auto instanceBuff = ctx.resources.RequestUnsafeVertexBuffer(mesh->GetVertexCount() * sizeof(InstanceBuff));

			auto mapped = instanceBuff->LockSpan<InstanceBuff>(mesh->GetVertexCount());

			for (uint32 i = 0; i < mesh->GetVertexCount(); i++)
			{
				mapped[i].pos = transform.GetLocalPosition();
				// mapped[i].pos = transform.GetWorldPosition() + mesh->GetVertices()[i].position;
				mapped[i].size = { 0.05f, 0.05f };
			}

			instanceBuff->Unlock();
			
			cmd->BindPipeline({
				.vertexShaderModules = { shaderModule },
				.pixelShaderModules = { shaderModule },
				.cullMode = CullMode::None,
			});

			cmd->BindUniformBuffer("Info", infoBuff);

			// cmd->BindVertexBuffers({ m_vertexBuffer, instanceBuff });

			// cmd->BindIndexBuffer(m_indexBuffer);

			// cmd->DrawIndexed(m_indexBuffer->GetCount(), mesh->GetVertexCount());
			
			
			cmd->BindVertexBuffers({ mesh->GetVertexBuffer(), instanceBuff });
			cmd->BindIndexBuffer(mesh->GetIndexBuffer());

			cmd->DrawIndexed(mesh->GetIndexBuffer()->GetCount(), 1);
			
			cmd->EndRenderPass();

			cmd->Execute();
		});	
	}
	
	void ScenePanel::RenderFramebuffer(NativeCommandList& cmdList)
	{
		Graphics::GetMainRenderGraph()->PushView({
			.size = m_EditorCamera.GetViewportSize(),
			.family = RenderView::Family::Scene,
			.viewTransform = m_EditorCamera.GetViewProjection(),
		});

		auto& scenes = SceneRegistry::GetActiveScenes();

		for (auto& scene : scenes)
		{
			auto cameraView = scene->GetComponentsWith<CameraComponent, TransformComponent>();
			
			for (auto _entity : cameraView)
			{
				auto entity = scene->GetEntity(_entity);

				auto camera = entity.GetComponent<CameraComponent>();
				auto transform = Transform(entity);

				Graphics::GetMainRenderGraph()->PushView({
					.size = { 300, 300 },
					.family = RenderView::Family::Scene,
					.viewTransform = transform.GetWorldMatrix(),
				});
			}

			HL_PROFILE_BEGIN("RenderGraph - Collecting Objects 3D");

			auto startTime = Time::CurrentTimeMicroseconds();

			auto meshView = scene->GetComponentsWith<TransformComponent, MeshRendererComponent>();

			EntityContainer* container = scene.get();
			uint64 count = 0;
			for (auto _entity : meshView)
			{
				count++;

				// auto entity = scene->GetEntity(_entity);

				auto entity = EntityHot(_entity, container);

				// auto transform = Transform(entity);
				// auto& meshRenderer = entity.GetComponent<MeshRendererComponent>();
				auto& meshRenderer = container->GetComponent<MeshRendererComponent>(_entity);
				auto& transform = container->GetComponent<TransformComponent>(_entity);

				Graphics::GetMainRenderGraph()->PushObject({
					.transform = Matrix4x4::Identity(),
					.mesh = meshRenderer.mesh,
					.material = meshRenderer.material,
				});
			}

			auto endTime = Time::CurrentTimeMicroseconds();

			auto duration = (endTime - startTime) / 1000.f;

			std::cout << "RenderGraph - Collecting Objects 3D: " << duration << " ms - " << std::to_string(count) << " objects" << std::endl;

			HL_PROFILE_END();
		}
	}

	void ScenePanel::ResetControls()
	{
		m_EditorCamera.Reset();
	}

	// bool ScenePanel::ToggleTool(GizmosRenderer::Tool tool)
	// {
	// 	auto test = InspectorPanel::GetInstance().GetType();

	// 	if (InspectorPanel::GetInstance().GetType() != InspectorPanel::SelectedType::Entity) return false;

	// 	Entity obj = std::any_cast<Entity>(InspectorPanel::GetInstance().GetHandle());

	// 	if (!obj.IsValid()) return false;

	// 	Transform transform = Transform(obj);

	// 	tool_data.world_transform = transform.GetWorldTransformCache();
	// 	tool_data.local_transform = transform.GetTransformComponent();
	// 	tool_data.entity = obj;
	// 	tool_data.tool = tool;
	// 	return true;
	// }

	void ScenePanel::HandleTool(float x, float y, bool start)
	{
		// auto& transform = tool_data.entity.GetComponent<TransformComponent>();

		// switch (tool_data.tool)
		// {

		// case GizmosRenderer::Tool::RotateX:
		// case GizmosRenderer::Tool::RotateY:
		// case GizmosRenderer::Tool::RotateZ:
		// {
		// 	Vector2 obj_screen_pos = m_EditorCamera.WorldToScreenPoint(tool_data.world_transform.Position);
		// 	Vector2 mouse_pos = Vector2{ x ,  y };

		// 	static Vector2 first_pos;

		// 	Vector2 obj_screen_x;
		// 	Vector2 obj_screen_y;

		// 	switch (tool_data.tool)
		// 	{
		// 		case GizmosRenderer::Tool::RotateX:
		// 		obj_screen_x = m_EditorCamera.WorldToScreenPoint(tool_data.world_transform.Position + tool_data.world_transform.Up());
		// 		obj_screen_y = m_EditorCamera.WorldToScreenPoint(tool_data.world_transform.Position + tool_data.world_transform.Forward());
		// 		break;
		// 		case GizmosRenderer::Tool::RotateY:
		// 		obj_screen_x = m_EditorCamera.WorldToScreenPoint(tool_data.world_transform.Position + tool_data.world_transform.Right());
		// 		obj_screen_y = m_EditorCamera.WorldToScreenPoint(tool_data.world_transform.Position + tool_data.world_transform.Forward());
		// 		break;
		// 		case GizmosRenderer::Tool::RotateZ:
		// 		obj_screen_x = m_EditorCamera.WorldToScreenPoint(tool_data.world_transform.Position + tool_data.world_transform.Right());
		// 		obj_screen_y = m_EditorCamera.WorldToScreenPoint(tool_data.world_transform.Position + tool_data.world_transform.Up());
		// 		break;
		// 		default: break;
		// 	}
			
		// 	Vector2 y_axis = (obj_screen_pos - obj_screen_x).normalize();
		// 	Vector2 x_axis = (obj_screen_y - obj_screen_pos).normalize();

		// 	Vector2 y_project = obj_screen_pos + y_axis * Vector2::Dot(y_axis, mouse_pos - obj_screen_pos);
		// 	Vector2 x_project = obj_screen_pos + x_axis * Vector2::Dot(x_axis, mouse_pos - obj_screen_pos);

		// 	Vector2 mouse_position_on_graph = Vector2 { Vector2::Distance(x_project, obj_screen_pos) * (Vector2::Dot(x_axis, x_project - obj_screen_pos) < 0 ? -1.f : 1.f) , Vector2::Distance(y_project, obj_screen_pos) * (Vector2::Dot(y_axis, y_project - obj_screen_pos) > 0 ? -1.f : 1.f) };

		// 	if (start)
		// 	{
		// 		first_pos = Vector2{ Vector2::Distance(x_project, obj_screen_pos) * (Vector2::Dot(x_axis, x_project - obj_screen_pos) < 0 ? -1.f : 1.f) , Vector2::Distance(y_project, obj_screen_pos) * (Vector2::Dot(y_axis, y_project - obj_screen_pos) > 0 ? -1.f : 1.f) };
		// 	}

		// 	float angle = std::atan2(first_pos.y, first_pos.x) - std::atan2(mouse_position_on_graph.y, mouse_position_on_graph.x);


		// 	switch (tool_data.tool)
		// 	{
		// 		case GizmosRenderer::Tool::RotateX:
		// 		transform.Rotation = Quaternion::FromAxisAngle(Vector3::Right(), angle) * tool_data.local_transform.Rotation;
		// 		break;
		// 		case GizmosRenderer::Tool::RotateY:
		// 		transform.Rotation = Quaternion::FromAxisAngle(Vector3::Up(), -angle) * tool_data.local_transform.Rotation;
		// 		break;
		// 		case GizmosRenderer::Tool::RotateZ:
		// 		transform.Rotation = Quaternion::FromAxisAngle(Vector3::Forward(), angle) * tool_data.local_transform.Rotation;
		// 		break;
		// 		default: break;
		// 	}

		// 	ImVec2 imgPos = { 0, 20 };

		// 	ImGui::SetCursorPos(ImVec2(imgPos.x + obj_screen_pos.x, imgPos.y + obj_screen_pos.y));
		// 	ImGui::Text("Location: %f, %f", obj_screen_pos.x, obj_screen_pos.y);

		// 	ImGui::SetCursorPos(ImVec2(imgPos.x + obj_screen_x.x, imgPos.y + obj_screen_x.y));
		// 	ImGui::Text("Up: %f, %f", obj_screen_x.x, obj_screen_x.y);

		// 	ImGui::SetCursorPos(ImVec2(imgPos.x + obj_screen_y.x, imgPos.y + obj_screen_y.y));
		// 	ImGui::Text("Forward: %f, %f", obj_screen_y.x, obj_screen_y.y);

		// 	ImGui::SetCursorPos(ImVec2(imgPos.x + y_project.x, imgPos.y + y_project.y));
		// 	ImGui::Text("Y: %f, %f", y_project.x, y_project.y);

		// 	ImGui::SetCursorPos(ImVec2(imgPos.x + x_project.x, imgPos.y + x_project.y));
		// 	ImGui::Text("Z: %f, %f", x_project.x, x_project.y);

		// 	ImGui::SetCursorPos(ImVec2(imgPos.x + mouse_position_on_graph.x, imgPos.y + mouse_position_on_graph.y));
		// 	ImGui::Text("Mouse: %f, %f", mouse_position_on_graph.x, mouse_position_on_graph.y);

		// 	ImGui::SetCursorPos(ImVec2(imgPos.x + first_pos.x, imgPos.y + first_pos.y));
		// 	ImGui::Text("First: %f, %f", first_pos.x, first_pos.y);

		// 	ImGui::SetCursorPos(ImVec2(imgPos.x + mouse_pos.x, imgPos.y + mouse_pos.y));
		// 	ImGui::Text("Mouse: %f, %f", mouse_pos.x, mouse_pos.y);

		// 	ImGui::SetCursorPos(ImVec2(imgPos.x + obj_screen_pos.x, imgPos.y + obj_screen_pos.y));
		// 	ImGui::Text("Angle: %f", angle);

		// 	break;
		// }
		// /*case GizmosRenderer::Tool::RotateX:
		// {
		// 	Vector2 obj_screen_pos = m_EditorCamera.WorldToScreenPoint(tool_data.world_transform.Position);
		// 	Vector2 mouse_pos = Vector2{ x ,  y };

		// 	static Vector2 first_pos;

		// 	Vector2 obj_screen_up = m_EditorCamera.WorldToScreenPoint(tool_data.world_transform.Position + tool_data.world_transform.Up());
		// 	Vector2 obj_screen_forward = m_EditorCamera.WorldToScreenPoint(tool_data.world_transform.Position + tool_data.world_transform.Forward());

		// 	Vector2 y_axis = (obj_screen_pos - obj_screen_up).normalize();
		// 	Vector2 z_axis = (obj_screen_forward - obj_screen_pos).normalize();

		// 	Vector2 y_project = obj_screen_pos + y_axis * Vector2::Dot(y_axis, mouse_pos - obj_screen_pos);
		// 	Vector2 z_project = obj_screen_pos + z_axis * Vector2::Dot(z_axis, mouse_pos - obj_screen_pos);

		// 	Vector2 mouse_position_on_graph = Vector2 { Vector2::Distance(z_project, obj_screen_pos) * (Vector2::Dot(z_axis, z_project - obj_screen_pos) < 0 ? -1.f : 1.f) , Vector2::Distance(y_project, obj_screen_pos) * (Vector2::Dot(y_axis, y_project - obj_screen_pos) > 0 ? -1.f : 1.f) };

		// 	if (start)
		// 	{
		// 		first_pos = Vector2{ Vector2::Distance(z_project, obj_screen_pos) * (Vector2::Dot(z_axis, z_project - obj_screen_pos) < 0 ? -1.f : 1.f) , Vector2::Distance(y_project, obj_screen_pos) * (Vector2::Dot(y_axis, y_project - obj_screen_pos) > 0 ? -1.f : 1.f) };
		// 	}

		// 	float angle = std::atan2(first_pos.y, first_pos.x) - std::atan2(mouse_position_on_graph.y, mouse_position_on_graph.x);

		// 	transform.Rotation = Quaternion::FromAxisAngle(Vector3::Right(), angle) * tool_data.local_transform.Rotation;

		// 	ImVec2 imgPos = { 0, 20 };

		// 	ImGui::SetCursorPos(ImVec2(imgPos.x + obj_screen_pos.x, imgPos.y + obj_screen_pos.y));
		// 	ImGui::Text("Location: %f, %f", obj_screen_pos.x, obj_screen_pos.y);
		// 	ImGui::SetCursorPosX( imgPos.x + obj_screen_pos.x );
		// 	ImGui::Text("y_axis: %f, %f", y_axis.x, y_axis.y);
		// 	ImGui::SetCursorPosX( imgPos.x + obj_screen_pos.x );
		// 	ImGui::Text("z_axis: %f, %f", z_axis.x, z_axis.y);

		// 	ImGui::SetCursorPos(ImVec2(imgPos.x + x, imgPos.y + y));
		// 	ImGui::Text("Mouse: %f, %f", x, y);
		// 	ImGui::SetCursorPosX( imgPos.x + x );
		// 	ImGui::Text("X: %f, Y: %f", mouse_position_on_graph.x, mouse_position_on_graph.y);
		// 	ImGui::SetCursorPosX( imgPos.x + x );
		// 	ImGui::Text("y_project: %f, %f", y_project.x, y_project.y);
		// 	ImGui::SetCursorPosX( imgPos.x + x );
		// 	ImGui::Text("z_project: %f, %f", z_project.x, z_project.y);
		// 	ImGui::SetCursorPosX( imgPos.x + x );
		// 	ImGui::Text("angle: %f", angle);
		// 	ImGui::Text("Angle Deg: %f", angle * 180 / PI);

		// 	Vector3 point_y = m_EditorCamera.ScreenToWorldPoint(y_project);
		// 	Vector3 point_z = m_EditorCamera.ScreenToWorldPoint(z_project);

		// 	GizmosRenderer::DrawLine(tool_data.world_transform.Position, point_y, 0.5f, Color::Bisque);
		// 	GizmosRenderer::DrawLine(tool_data.world_transform.Position, point_z, 0.5f, Color::BlanchedAlmond);






		// 	GizmosRenderer::DrawLine(Transform(tool_data.entity).GetWorldPositionCache(), m_EditorCamera.ScreenToWorldPoint(x, y, 1.0f), 0.8f, Color::BlanchedAlmond, -1, GizmosRenderer::LineMode::Rounded_Dashed);

		// 	break;
		// }*/
		// case GizmosRenderer::Tool::RotateXYZ:
		// {
		// 	Vector2 obj_screen_pos = m_EditorCamera.WorldToScreenPoint(tool_data.world_transform.Position);
		// 	Vector2 mouse_pos_dir = Vector2{ x - obj_screen_pos.x, obj_screen_pos.y - y }.normalize();

		// 	static Vector2 mouse_first_pos_dir;
		// 	if (start) mouse_first_pos_dir = Vector2{ x - obj_screen_pos.x, obj_screen_pos.y - y }.normalize();

		// 	float angle = std::atan2f(mouse_first_pos_dir.x, mouse_first_pos_dir.y) - std::atan2f(mouse_pos_dir.x, mouse_pos_dir.y);

		// 	ImVec2 imgPos = { 0, 20 };

		// 	ImGui::SetCursorPos(ImVec2(imgPos.x + obj_screen_pos.x, imgPos.y + obj_screen_pos.y));
		// 	ImGui::Text("Location: %f, %f", obj_screen_pos.x, obj_screen_pos.y);

		// 	ImGui::SetCursorPos(ImVec2(imgPos.x + x, imgPos.y + y));
		// 	ImGui::Text("Mouse: %f, %f", x, y);
			
		// 	ImGui::SetCursorPos(ImVec2(imgPos.x + x, imgPos.y + y + 15));
		// 	ImGui::Text("Dir: %f, %f", mouse_pos_dir.x, mouse_pos_dir.y);

		// 	ImGui::SetCursorPos(ImVec2(imgPos.x + x, imgPos.y + y + 30));
		// 	ImGui::Text("Angle: %f", angle);

		// 	ImGui::SetCursorPos(ImVec2(imgPos.x + x, imgPos.y + y + 45));
		// 	ImGui::Text("Angle Deg: %f", angle * 180 / PI);

		// 	transform.Rotation = Quaternion::FromAxisAngle(m_EditorCamera.GetForward(), angle) * tool_data.local_transform.Rotation;

		// 	GizmosRenderer::DrawLine(Transform(tool_data.entity).GetWorldPositionCache(), m_EditorCamera.ScreenToWorldPoint(x, y, 1.0f), 0.8f, Color::BlanchedAlmond, -1, GizmosRenderer::LineMode::Rounded_Dashed);
		// 	break;
		// }
		// case GizmosRenderer::Tool::TranslateXYZ:
		// {
		// 	auto cameraForward = m_EditorCamera.GetForward();
		// 	auto direction = (tool_data.world_transform.Position - m_EditorCamera.GetPosition()).normalize();

		// 	float alpha = std::acos(Vector3::Dot(cameraForward, direction)) / (cameraForward.magnitude() * direction.magnitude());
		// 	float C = Vector3::Distance(tool_data.world_transform.Position, m_EditorCamera.GetPosition());

		// 	float A = C * std::sin(alpha);
		// 	float B = std::sqrt(std::pow(C, 2.0f) - std::pow(A, 2.0f));

		// 	auto newLocation = m_EditorCamera.ScreenToWorldPoint(x, y, B);

		// 	GizmosRenderer::DrawLine(tool_data.world_transform.Position, newLocation, 0.8f, Color::BlanchedAlmond, -1, GizmosRenderer::LineMode::Rounded_Dashed);
		// 	transform.SetLocalPosition(newLocation);

		// 	break;
		// }
		// case GizmosRenderer::Tool::ScaleXYZ:
		// {
		// 	static float scale_start = 1.0f;

		// 	auto cameraForward = m_EditorCamera.GetForward();
		// 	auto direction = (tool_data.world_transform.Position - m_EditorCamera.GetPosition()).normalize();

		// 	float alpha = std::acos(Vector3::Dot(cameraForward, direction)) / (cameraForward.magnitude() * direction.magnitude());
		// 	float C = Vector3::Distance(tool_data.world_transform.Position, m_EditorCamera.GetPosition());

		// 	float A = C * std::sin(alpha);
		// 	float B = std::sqrt(std::pow(C, 2.0f) - std::pow(A, 2.0f));

		// 	auto newLocation = m_EditorCamera.ScreenToWorldPoint(x, y, B);

		// 	float increaseInScale = Vector3::Distance(tool_data.world_transform.Position, newLocation);

		// 	if (start)
		// 	{
		// 		scale_start = increaseInScale;
		// 	}

		// 	bool decrease = Vector3::Dot(m_EditorCamera.WorldToScreenPoint(tool_data.world_transform.Position), direction) < 0.0f;

		// 	GizmosRenderer::DrawLine(tool_data.world_transform.Position, newLocation, 0.8f, Color::BlanchedAlmond, -1, GizmosRenderer::LineMode::Rounded_Dashed);
		// 	transform.SetLocalScale(tool_data.local_transform.Scale * ((decrease ? scale_start - increaseInScale : increaseInScale - scale_start) + 1.0f));

		// 	break;
		// }
		// case GizmosRenderer::Tool::ScaleX:
		// case GizmosRenderer::Tool::ScaleY:
		// case GizmosRenderer::Tool::ScaleZ:
		// {
		// 	Vector3 pointA3D = tool_data.world_transform.Position;

		// 	Vector3 pointB3D;
		// 	Vector3 pointC3D;

		// 	switch (tool_data.tool)
		// 	{
		// 		case GizmosRenderer::Tool::ScaleX:
		// 		pointB3D = tool_data.world_transform.Position + tool_data.world_transform.Right();
		// 		pointC3D = tool_data.world_transform.Position - tool_data.world_transform.Right();
		// 		break;
		// 		case GizmosRenderer::Tool::ScaleY:
		// 		pointB3D = tool_data.world_transform.Position + tool_data.world_transform.Up();
		// 		pointC3D = tool_data.world_transform.Position - tool_data.world_transform.Up();
		// 		break;
		// 		case GizmosRenderer::Tool::ScaleZ:
		// 		pointB3D = tool_data.world_transform.Position + tool_data.world_transform.Forward();
		// 		pointC3D = tool_data.world_transform.Position - tool_data.world_transform.Forward();
		// 		break;
		// 		default: break;
		// 	}

		// 	Vector2 pointA = m_EditorCamera.WorldToScreenPoint(pointA3D);
		// 	Vector2 pointB = m_EditorCamera.WorldToScreenPoint(pointB3D);
		// 	Vector2 pointC = m_EditorCamera.WorldToScreenPoint(pointC3D);

		// 	GizmosRenderer::DrawLine(pointA3D, pointB3D, 0.8f, Color::BlanchedAlmond, -1, GizmosRenderer::LineMode::Solid);

		// 	Vector2 mouse = { x, y };
		// 	Vector2 dir = (pointB - pointA).normalize();
		// 	Vector2 m_dir = (mouse - pointA).normalize();

		// 	bool reversed = Vector2::Dot(m_dir, dir) < 0;

		// 	float distance = Vector2::Distance(pointA, pointB);
		// 	float distancec = Vector2::Distance(pointA,  pointC);

		// 	Vector2 proj = pointA + dir * Vector2::Dot(mouse - pointA, dir);

		// 	Vector3 wProj = m_EditorCamera.ScreenToWorldPoint(proj.x, proj.y, 1.0f);


		// 	float increased = (Vector3::Distance(proj, pointA) / distance);
		// 	//if (reversed) increased *= -1;
		// 	if (reversed) increased = Vector3::Distance(proj, pointA) / distancec * -1;

		// 	GizmosRenderer::DrawLine(m_EditorCamera.ScreenToWorldPoint(pointA, 1.0f), m_EditorCamera.ScreenToWorldPoint(pointA + dir), 14.8f, Color::Orange, -1, GizmosRenderer::LineMode::Solid);

		// 	//GizmosRenderer::DrawLine(pointA3D, pointA3D + tool_data.world_transform.Right() * increased, 4.8f, Color::Blue, -1, GizmosRenderer::LineMode::Solid);


		// 	if (reversed)
		// 		GizmosRenderer::DrawLine(m_EditorCamera.ScreenToWorldPoint(pointA + dir * (increased * distance), 1.9f), pointA3D, 0.8f, Color::Blue, -1, GizmosRenderer::LineMode::Solid);
		// 	else 
		// 		GizmosRenderer::DrawLine(pointA3D, m_EditorCamera.ScreenToWorldPoint(pointA + dir * (increased * distance), 1.9f), 0.8f, Color::Blue, -1, GizmosRenderer::LineMode::Solid);

		// 	GizmosRenderer::DrawLine(pointA3D, wProj, 0.8f, Color::Green, -1, GizmosRenderer::LineMode::Solid);

		// 	Vector3 scale = tool_data.local_transform.Scale;

		// 	switch (tool_data.tool)
		// 	{
		// 	case GizmosRenderer::Tool::ScaleX:
		// 		GizmosRenderer::DrawLine(tool_data.world_transform.Right() * 1000.0f + tool_data.world_transform.Position, tool_data.world_transform.Right() * -1000.0f + tool_data.world_transform.Position, 0.5f, Color::Red, -1, GizmosRenderer::LineMode::Solid);
		// 		scale.x *= increased;
		// 		break;
		// 	case GizmosRenderer::Tool::ScaleY:
		// 		GizmosRenderer::DrawLine(tool_data.world_transform.Up() * 1000.0f + tool_data.world_transform.Position, tool_data.world_transform.Up() * -1000.0f + tool_data.world_transform.Position, 0.5f, Color::Blue, -1, GizmosRenderer::LineMode::Solid);
		// 		scale.y *= increased;
		// 		break;
		// 	case GizmosRenderer::Tool::ScaleZ:
		// 		GizmosRenderer::DrawLine(tool_data.world_transform.Forward() * 1000.0f + tool_data.world_transform.Position, tool_data.world_transform.Forward() * -1000.0f + tool_data.world_transform.Position, 0.5f, Color::Green, -1, GizmosRenderer::LineMode::Solid);
		// 		scale.z *= increased;
		// 		break;
		// 	default: break;
		// 	}

		// 	transform.SetLocalScale(scale);

		// 	break;
		// }
		// case GizmosRenderer::Tool::TranslateX:
		// case GizmosRenderer::Tool::TranslateY:
		// case GizmosRenderer::Tool::TranslateZ:
		// {
		// 	Vector3 pointA3D = tool_data.world_transform.Position;

		// 	Vector3 pointB3D;
		// 	Vector3 pointC3D;

		// 	switch (tool_data.tool)
		// 	{
		// 		case GizmosRenderer::Tool::TranslateX:
		// 		pointB3D = tool_data.world_transform.Position + tool_data.world_transform.Right();
		// 		pointC3D = tool_data.world_transform.Position - tool_data.world_transform.Right();
		// 		break;
		// 		case GizmosRenderer::Tool::TranslateY:
		// 		pointB3D = tool_data.world_transform.Position + tool_data.world_transform.Up();
		// 		pointC3D = tool_data.world_transform.Position - tool_data.world_transform.Up();
		// 		break;
		// 		case GizmosRenderer::Tool::TranslateZ:
		// 		pointB3D = tool_data.world_transform.Position + tool_data.world_transform.Forward();
		// 		pointC3D = tool_data.world_transform.Position - tool_data.world_transform.Forward();
		// 		break;
		// 		default: break;
		// 	}

		// 	Vector2 pointA = m_EditorCamera.WorldToScreenPoint(pointA3D);
		// 	Vector2 pointB = m_EditorCamera.WorldToScreenPoint(pointB3D);
		// 	Vector2 pointC = m_EditorCamera.WorldToScreenPoint(pointC3D);

		// 	GizmosRenderer::DrawLine(pointA3D, pointB3D, 0.8f, Color::BlanchedAlmond, -1, GizmosRenderer::LineMode::Solid);

		// 	Vector2 mouse = { x, y };
		// 	Vector2 dir = (pointB - pointA).normalize();
		// 	Vector2 m_dir = (mouse - pointA).normalize();

		// 	bool reversed = Vector2::Dot(m_dir, dir) < 0;

		// 	float distance = Vector2::Distance(pointA, pointB);
		// 	float distancec = Vector2::Distance(pointA,  pointC);

		// 	Vector2 proj = pointA + dir * Vector2::Dot(mouse - pointA, dir);

		// 	Vector3 wProj = m_EditorCamera.ScreenToWorldPoint(proj.x, proj.y, 1.0f);


		// 	float increased = (Vector3::Distance(proj, pointA) / distance);
		// 	//if (reversed) increased *= -1;
		// 	if (reversed) increased = Vector3::Distance(proj, pointA) / distancec * -1;

		// 	GizmosRenderer::DrawLine(m_EditorCamera.ScreenToWorldPoint(pointA, 1.0f), m_EditorCamera.ScreenToWorldPoint(pointA + dir), 14.8f, Color::Orange, -1, GizmosRenderer::LineMode::Solid);

		// 	//GizmosRenderer::DrawLine(pointA3D, pointA3D + tool_data.world_transform.Right() * increased, 4.8f, Color::Blue, -1, GizmosRenderer::LineMode::Solid);


		// 	if (reversed)
		// 		GizmosRenderer::DrawLine(m_EditorCamera.ScreenToWorldPoint(pointA + dir * (increased * distance), 1.9f), pointA3D, 0.8f, Color::Blue, -1, GizmosRenderer::LineMode::Solid);
		// 	else 
		// 		GizmosRenderer::DrawLine(pointA3D, m_EditorCamera.ScreenToWorldPoint(pointA + dir * (increased * distance), 1.9f), 0.8f, Color::Blue, -1, GizmosRenderer::LineMode::Solid);

		// 	GizmosRenderer::DrawLine(pointA3D, wProj, 0.8f, Color::Green, -1, GizmosRenderer::LineMode::Solid);

		// 	//GizmosRenderer::DrawLine(pointA3D, pointA3D + tool_data.world_transform.Right() * increased, 0.8f, Color::Green, -1, GizmosRenderer::LineMode::Solid);
		// 	//GizmosRenderer::DrawLine(pointA3D, pointA3D + tool_data.world_transform.Right(), 1.8f, Color::Yellow, -1, GizmosRenderer::LineMode::Solid);


		// 	switch (tool_data.tool)
		// 	{
		// 	case GizmosRenderer::Tool::TranslateX:
		// 		transform.SetLocalPosition(tool_data.world_transform.Position + tool_data.world_transform.Right() * increased);
		// 		GizmosRenderer::DrawLine(tool_data.world_transform.Right() * 1000.0f + tool_data.world_transform.Position, tool_data.world_transform.Right() * -1000.0f + tool_data.world_transform.Position, 0.5f, Color::Red, -1, GizmosRenderer::LineMode::Solid);
		// 		break;
		// 	case GizmosRenderer::Tool::TranslateY:
		// 		transform.SetLocalPosition(tool_data.world_transform.Position + tool_data.world_transform.Up() * increased);
		// 		GizmosRenderer::DrawLine(tool_data.world_transform.Up() * 1000.0f + tool_data.world_transform.Position, tool_data.world_transform.Up() * -1000.0f + tool_data.world_transform.Position, 0.5f, Color::Blue, -1, GizmosRenderer::LineMode::Solid);
		// 		break;
		// 	case GizmosRenderer::Tool::TranslateZ:
		// 		transform.SetLocalPosition(tool_data.world_transform.Position + tool_data.world_transform.Forward() * increased);
		// 		GizmosRenderer::DrawLine(tool_data.world_transform.Forward() * 1000.0f + tool_data.world_transform.Position, tool_data.world_transform.Forward() * -1000.0f + tool_data.world_transform.Position, 0.5f, Color::Green, -1, GizmosRenderer::LineMode::Solid);
		// 		break;
		// 	default: break;
		// 	}
			
		// 	ImGui::SetCursorPos(ImVec2(20, 40));
		// 	ImGui::Text("Increased: %f", increased);
		// 	ImGui::Text("Max Distance: %f", Vector3::Distance(proj, pointA));
		// 	ImGui::Text("Distance: %f", distance);
		// 	ImGui::Text("DistanceC: %f", distancec);

		// 	break;
		// }
		// case GizmosRenderer::Tool::None:
		// default:
		// 	break;
		// }
	}


	void ScenePanel::OnUpdate()
	{
		static bool show_fps = false;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		if(ImGui::Begin(GetName().c_str(), &m_window_open, ImGuiWindowFlags_MenuBar))
		{
			// if (ImGui::BeginMenuBar())
			// {
			// 	if (active_tool_type == GizmosRenderer::ToolType::Grab) ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
			// 	if (ImGui::Button("G")) active_tool_type = GizmosRenderer::ToolType::Grab;
			// 	else
			// 		if (active_tool_type == GizmosRenderer::ToolType::Grab) ImGui::PopStyleColor();

			// 	if (active_tool_type == GizmosRenderer::ToolType::Move) ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
			// 	if (ImGui::Button("M")) active_tool_type = GizmosRenderer::ToolType::Move;
			// 	else
			// 		if (active_tool_type == GizmosRenderer::ToolType::Move) ImGui::PopStyleColor();

			// 	if (active_tool_type == GizmosRenderer::ToolType::Rotate) ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
			// 	if (ImGui::Button("R")) active_tool_type = GizmosRenderer::ToolType::Rotate;
			// 	else
			// 		if (active_tool_type == GizmosRenderer::ToolType::Rotate) ImGui::PopStyleColor();

			// 	if (active_tool_type == GizmosRenderer::ToolType::Scale) ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
			// 	if (ImGui::Button("S")) active_tool_type = GizmosRenderer::ToolType::Scale;
			// 	else
			// 		if (active_tool_type == GizmosRenderer::ToolType::Scale) ImGui::PopStyleColor();

			// 	ImGui::Checkbox("Show ID Frame", &showIdFrame);
			// 	ImGui::Checkbox("Show Depth Buffer", &showDepth);
			// 	ImGui::Checkbox("Show FPS", &show_fps);

			// 	bool isVsync = Helios::Application::GetInstance().GetWindow()->VSyncEnabled();

			// 	if (ImGui::Checkbox("VSync", &isVsync))
			// 	{
			// 		Helios::Application::GetInstance().GetWindow()->SetVSync(isVsync);
			// 	}

			// 	//static bool wireframe = false;
			// 	//ImGui::Checkbox("Wireframe", &wireframe);
			// 	/*if (wireframe)
			// 			this->currentRSState = RSState::Wireframe;
			// 		else
			// 			this->currentRSState = RSState::Normal;*/


			// 	ImGui::Checkbox("Show Vertecies", &show_vertecies);

			// 	ImGui::EndMenuBar();
			// }

			auto imgPos = ImGui::GetCursorPos();
			auto mousePos = ImGui::GetMousePos();
			auto windowPos = ImGui::GetWindowPos();
			auto x = mousePos.x - windowPos.x - imgPos.x;
			auto y = mousePos.y - windowPos.y - imgPos.y;

			ImVec2 viewportSize = ImGui::GetContentRegionAvail();
			auto currentvpsize = m_colorImage->GetSize();
			if ((viewportSize.x != currentvpsize.width || viewportSize.y != currentvpsize.height) && viewportSize.x > 0 && viewportSize.y > 0 && viewportSize.x < 10000 && viewportSize.y < 10000)
			{
				m_colorImage->Resize((uint32_t)viewportSize.x, (uint32_t)viewportSize.y);
				m_EditorCamera.SetViewportSize(viewportSize.x, viewportSize.y);
			}
			if (showDepth)
			{
				
			}
				// TODO: change to render to tmp texture
				// ImGui::Image(m_Framebuffer->GetDepthTextureID(), ImVec2(viewportSize), ImVec2(0, 0), ImVec2(1, 1), ImVec4(1, 0,0,1));
			// else
				ImGui::Image(m_colorImage->GetID(), ImVec2(viewportSize));

			if (show_fps)
			{
				// Show Avarage FPS
				ImGui::SetCursorPos(ImVec2(20, 60));

				static float fps[100];

				static int fps_index = 0;

				fps[fps_index] = Time::FramesPerSecond();

				fps_index++;

				if (fps_index >= 100)
					fps_index = 0;

				float fps_sum = 0.0f;

				for (int i = 0; i < 100; i++)
					fps_sum += fps[i];

				float fps_avg = fps_sum / 100.0f;

				ImGui::Text("FPS: %f", fps_avg);
			}


			static ImVec2 lastMousePos = ImGui::GetMousePos();
			Vector2 delta = Vector2(mousePos.x - lastMousePos.x, mousePos.y - lastMousePos.y);
			lastMousePos = mousePos;
			
			ImGui::Begin("Mouse Location Stats");

			ImGui::Text("Mouse Position: (%.2f, %.2f)", mousePos.x, mousePos.y);
			ImGui::Text("Delta: (%.2f, %.2f)", delta.x, delta.y);

			ImGui::Text("Mouse X: %f, Y: %f", x, y);

			// TODO: Reimplement GetPixel
			// if (x >= 0 && y >= 0 && x < m_colorImage->GetWidth() && y < m_colorImage->GetHeight()) {

			// 	Int4 clicked_pixel = m_colorImage->GetPixelInt4(1u, x, y);
				
			// 	uint64_t entityId = ((*(uint64_t*)&clicked_pixel.x) & 0xffffffff) | ((*(uint64_t*)&clicked_pixel.y) << 32);

			// 	ImGui::Text("ID: %lu", entityId);
			// 	ImGui::Text("Mode: %d", clicked_pixel.z);
			// 	Color color = m_colorImage->GetPixelColor(0u, x, y);
			// 	ImGui::Text("R: %f, G: %f, B: %f, A: %f", color.r, color.g, color.b, color.a);
			// }
			// else {
			// 	ImGui::Text("ID: -");
			// 	ImGui::Text("R: -, G: -, B: -, A: -");
			// }

			ImGui::Text("Mouse to World: %s", m_EditorCamera.ScreenToWorldPoint(x, y, 5.0f).to_string().c_str());


			ImGui::End();



			// if (InspectorPanel::GetInstance().GetType() == InspectorPanel::SelectedType::Entity)
			// 		{

			// 			Entity entity = std::any_cast<Entity>(InspectorPanel::GetInstance().GetHandle());
			// 			if (entity.IsNotNull())
			// 			{
			// 				if (entity.HasComponent<TransformComponent>())
			// 				{
			// 					GizmosRenderer::DrawTool(Transform(entity), m_EditorCamera, active_tool_type, tool_data.tool);
			// 				}
			// 				if (entity.HasComponent<MeshRendererComponent>())
			// 				{
			// 					Ref<Mesh> mesh = entity.GetComponent<MeshRendererComponent>().mesh;
			// 					auto transform = Transform(entity);

			// 					auto dir = (transform.GetWorldPosition() - m_EditorCamera.GetPosition()).normalize();
			// 					auto cameraForward = m_EditorCamera.GetForward();

			// 					auto trans = entity.GetComponent<TransformComponent>();

			// 					auto proj = m_EditorCamera.GetViewProjection();

			// 					float alpha = std::acos(Vector3::Dot(cameraForward, dir)) / (cameraForward.magnitude() * dir.magnitude());
			// 					float C = Vector3::Distance(transform.GetWorldPosition(), m_EditorCamera.GetPosition());

			// 					float A = C * std::sin(alpha);
			// 					float B = std::sqrt(std::pow(C, 2.0f) - std::pow(A, 2.0f));

			// 					//float angle = std::acos(Vector3::Dot(camTrans.Position, trans.Position)) / (camTrans.Position.magnitude() * trans.Position.magnitude());
			// 					//if (ImGui::Begin("Mesh Stats"))
			// 					//{
			// 					//	ImGui::Text("Angle: %f", alpha);
			// 					//	ImGui::Text("C: %f, B: %f, A: %f", C, B, A);
			// 					//	ImGui::Text("Direction: %f %f %f", dir.x, dir.y, dir.z);
			// 					//	ImGui::Text("Forward: %f %f %f", cameraForward.x, cameraForward.y, cameraForward.z);

			// 					//	uint32_t indeciesCount = mesh->getIndexCount();

			// 					//	ImGui::Text("Vertex Count: %d", mesh->getVertexCount());
			// 					//	ImGui::Text("Index Count: %d", indeciesCount);

			// 					//	ImGui::Text(mesh->getIndexBuffer()->m_DataStr.c_str());

			// 					//}
			// 					//
			// 					//ImGui::End();


			// 					if (show_vertecies)
			// 					{
			// 						GizmosRenderer::DrawMeshVertices(m_EditorCamera, trans, mesh->GetVertices());
			// 					}
			// 				}
			// 			}
			// 		}


				static bool pressed = false;
				static ImVec2 origin;
				static TransformComponent originTransform;
				static TransformComponent originWorldTransform;
				static Entity originEntity;
				static bool first = true;

				// static bool start = false;
				// if (ImGui::IsWindowHovered() && !ImGui::IsMouseDown(ImGuiMouseButton_Right))
				// {
				// 	static bool hold = false;

				// 	if (ImGui::IsMouseDown(ImGuiMouseButton_Left))
				// 	{
				// 		if (tool_data.tool == GizmosRenderer::Tool::None && x >= 0 && y >= 0 && x < m_Framebuffer->GetWidth() && y < m_Framebuffer->GetHeight())
				// 		{
				// 			Int4 clicked_pixel = m_Framebuffer->GetPixelInt4(1u, x, y);
				// 			int32_t mode_or_index = clicked_pixel.z;

				// 			if (mode_or_index >= 0)
				// 			{
				// 				uint64_t entityId = ((*(uint64_t*)&clicked_pixel.x) & 0xffffffff) | ((*(uint64_t*)&clicked_pixel.y) << 32);

				// 				WeakRef<Scene> scene = SceneRegistry::GetSceneByIndex(mode_or_index);

				// 				Entity entity = { (entt::entity)entityId, scene };

				// 				InspectorPanel::GetInstance() << entity;
				// 				InspectorPanel::FocusMain();
				// 			}
				// 			else if (mode_or_index == -1)
				// 			{
				// 				InspectorPanel::GetInstance().Reset();
				// 			}
				// 			else if (tool_data.tool == GizmosRenderer::Tool::None)
				// 			{
				// 				if (ToggleTool(static_cast<GizmosRenderer::Tool>(mode_or_index)))
				// 				{
				// 					hold = true;
				// 					start = true;
				// 				}
				// 			}
				// 		} else if (!hold)
				// 		{
				// 			hold = false;
				// 			tool_data.tool = GizmosRenderer::Tool::None;
				// 			tool_data.entity = Entity();
				// 		}
				// 	} else if (hold)
				// 	{
				// 		hold = false;
				// 		tool_data.tool = GizmosRenderer::Tool::None;
				// 		tool_data.entity = Entity();
				// 	}

				// 	if (tool_data.tool == GizmosRenderer::Tool::None)
				// 	{
				// 		if (ImGui::IsKeyDown(ImGuiKey_R))
				// 		{
				// 			ToggleTool(GizmosRenderer::Tool::RotateXYZ); start = true;
				// 		}
				// 		else if (ImGui::IsKeyDown(ImGuiKey_G))
				// 		{
				// 			ToggleTool(GizmosRenderer::Tool::TranslateXYZ); start = true;
				// 		}
				// 		else if (ImGui::IsKeyDown(ImGuiKey_S))
				// 		{
				// 			ToggleTool(GizmosRenderer::Tool::ScaleXYZ); start = true;
				// 		}
				// 	} else if (ImGui::IsKeyDown(ImGuiKey_Escape) && tool_data.entity.IsValid())
				// 	{
				// 		tool_data.entity.GetComponent<TransformComponent>() = tool_data.local_transform;

				// 		tool_data.tool = GizmosRenderer::Tool::None;
				// 		tool_data.entity = Entity();
				// 		hold = false;
				// 	}
				// }

				// if (tool_data.tool != GizmosRenderer::Tool::None && tool_data.entity.IsValid()) { HandleTool(x, y, start); start = false; }

				// static bool notf = true;

				// // if (!SceneRegistry::GetCurrentScene().lock()->IsPrimaryCameraSet()) {
				// // 	if (notf) {
				// // 		auto pos = ImGui::GetWindowPos();
				// // 		auto Size = ImGui::GetWindowSize();
				// // 		ImGui::SetNextWindowFocus();
				// // 		ImGui::SetNextWindowBgAlpha(1.0f);
				// // 		ImGui::SetWindowFontScale(2);
				// // 		ImGui::SetNextWindowPos(ImVec2(pos.x + (Size.x / 2.0f) - 200.0f, pos.y + (Size.y / 2.0f) - 100.0f));
				// // 		ImGui::BeginChild("MissingCameraText", ImVec2(400, 200), true);
				// // 		//ImGui::Button("Test");
				// // 		//ImGui::SetCursorPos(ImVec2(0, 0));
				// // 		ImGui::Text("No CameraComponent Present for rendering!");
				// // 		if (ImGui::Button("Close")) {
				// // 			notf = false;
				// // 		}
				// // 		ImGui::EndChild();
				// // 	}
				// // }
				// // else notf = true;

				if (ImGui::IsWindowHovered())
				{
					static ImVec2 origin;
					static bool mActive = false;

					if (ImGui::IsMouseClicked(ImGuiMouseButton_Right))
					{
						ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
						origin = ImGui::GetMousePos();
						mActive = true;
					}



					if (mActive)
					{
						//std::cout << "Mouse Is Held" << std::endl;
						ImVec2 current = ImGui::GetMousePos();
						
						m_EditorCamera.HandleMovement(delta);

#ifdef HELIOS_PLATFORM_WINDOWS
						SetCursorPos(origin.x, origin.y);
#else
						// ImGui::SetCursorScreenPos(origin);
#endif
					}

					if (ImGui::IsMouseReleased(ImGuiMouseButton_Right)) {
						mActive = false;
						ImGui::SetMouseCursor(ImGuiMouseCursor_Arrow);
					}
				}

				if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) && ImGui::IsKeyDown(ImGuiKey_R)) {
					m_EditorCamera.Reset();
				}
				m_isVisible = true;
		}
		ImGui::End();
		ImGui::PopStyleVar();
		//ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		//if(ImGui::Begin("Scene", 0, ImGuiWindowFlags_MenuBar))
		//{
		

		//	if (InspectorPanel::GetInstance().GetType() == InspectorPanel::SelectedType::GameObject)
		//	{

		//		GameObject entity = std::any_cast<GameObject>(InspectorPanel::GetInstance().GetHandle());
		//		if (entity.IsNotNull())
		//		{
		//			GameObject gm = { entity,SceneRegistry::get_current_scene() };
		//			if (gm.HasComponent<MeshRendererComponent>())
		//			{
		//				Ref<Mesh> mesh = gm.GetComponent<MeshRendererComponent>().mesh;
		//				ImGui::Begin("Mesh Stats");
		//				auto transform = Transform(gm);

		//				auto dir = (transform.GetWorldPosition() - m_EditorCamera.GetPosition()).normalize();
		//				auto cameraForward = m_EditorCamera.GetForward();

		//				auto trans = gm.GetComponent<TransformComponent>();

		//				auto proj = m_EditorCamera.GetViewProjection();

		//				float alpha = std::acos(Vector3::Dot(cameraForward, dir)) / (cameraForward.magnitude() * dir.magnitude());
		//				float C = Vector3::Distance(transform.GetWorldPosition(), m_EditorCamera.GetPosition());

		//				float A = C * std::sin(alpha);
		//				float B = std::sqrt(std::pow(C, 2.0f) - std::pow(A, 2.0f));

		//				//float angle = std::acos(Vector3::Dot(camTrans.Position, trans.Position)) / (camTrans.Position.magnitude() * trans.Position.magnitude());

		//				ImGui::Text("Angle: %f", alpha);
		//				ImGui::Text("C: %f, B: %f, A: %f", C, B, A);
		//				ImGui::Text("Direction: %f %f %f", dir.x, dir.y, dir.z);
		//				ImGui::Text("Forward: %f %f %f", cameraForward.x, cameraForward.y, cameraForward.z);

		//				uint32_t indeciesCount = mesh->getIndexCount();

		//				ImGui::Text("Vertex Count: %d", mesh->getVertexCount());
		//				ImGui::Text("Index Count: %d", indeciesCount);

		//				ImGui::Text(mesh->getIndexBuffer()->m_DataStr.c_str());

		//				ImGui::End();
		//			}
		//		}
		//	}

		//	static bool pressed = false;
		//	static ImVec2 origin;
		//	static TransformComponent originTransform;
		//	static TransformComponent originWorldTransform;
		//	static GameObject originEntity;

		//	if (Helios::InputManager::IsKeyPressed(HL_KEY_MOUSE_LEFT) && ImGui::IsWindowHovered())
		//	//if (Helios::InputManager::IsKeyPressed(HL_KEY_MOUSE_LEFT) && ImGui::IsWindowFocused())
		//	{
		//		// Check if in bounds
		//		if (current_tool == GizmosRenderer::Tool::None && x >= 0 && y >= 0 && x < m_Framebuffer->GetWidth() && y < m_Framebuffer->GetHeight())
		//		{

		//			Color color = m_Framebuffer->GetPixel(0u, x, y);
		//			Color entId = m_Framebuffer->GetPixel(1u, x, y);
		//			int64_t id = std::round(entId.r);

		//			/*DepricatedHelios::ShowMessage("Pixel Clicked:",
		//					"X: " + std::to_string(x) +
		//					" Y: " + std::to_string(y) + "\n"
		//					"ID: " + std::to_string(entId.r) + "\n" +
		//					"R: " + std::to_string(color.r) +
		//					" G: " + std::to_string(color.g) +
		//					" B: " + std::to_string(color.b) +
		//					" A: " + std::to_string(color.a)
		//				);*/

		//			GameObject gm = SceneRegistry::GetPrimaryCamera();
		//			auto [trans, cam] = gm.GetComponent<TransformComponent, CameraComponent>();



		//			/*auto vec = editorCamera_old.ScreenToWorldCoordinates(x, y, 5.0f, m_Framebuffer->GetSize());
		//				std::cout << "x: " << vec.x << ", y: " << vec.y << ", z: " << vec.z << std::endl;*/
		//			

		//			if (id == -1) InspectorPanel::GetInstance().Reset();
		//			else if (id > -1)
		//			{
		//				entt::entity entity = (entt::entity)id;

		//				/*if (SceneRegistry::get_current_scene()->m_components.valid(entity))
		//					InspectorPanel::GetInstance() << GameObject{ (entt::entity)entity, SceneRegistry::get_current_scene().get() };*/
		//			}
		//			else if (id >= -13 || current_tool != GizmosRenderer::Tool::None)
		//			{
		//				if (static_cast<HeliosEditor&>(HeliosEditor::GetInstance()).inspector.GetType() == InspectorPanel::SelectedType::GameObject)
		//				{
		//					GameObject obj = std::any_cast<GameObject>(InspectorPanel::GetInstance().GetHandle());
		//					//TransformComponent& transform = obj.GetComponent<TransformComponent>();
		//					Transform transform = Transform(obj);

		//					if (!pressed)
		//					{
		//						origin = ImGui::GetMousePos();
		//						originTransform = transform.GetTransformComponent();
		//						originWorldTransform = transform.GetWorldTransformCache();
		//						pressed = true;
		//						current_tool = (GizmosRenderer::Tool)id;
		//						originEntity = obj;
		//					}
		//				}
		//			}
		//		}
		//	}
		//	else { pressed = false; current_tool = GizmosRenderer::Tool::None; }

		//	if (current_tool != GizmosRenderer::Tool::None && originEntity.IsValid())
		//	{
		//		Transform transform(originEntity);
		//		//TransformComponent& transform = originEntity.GetComponent<TransformComponent>();

		//		switch (current_tool)
		//		{
		//		case GizmosRenderer::Tool::MoveX:
		//			{
		//				auto cameraForward = m_EditorCamera.GetForward();
		//				auto direction = (originWorldTransform.Position - m_EditorCamera.GetPosition()).normalize();

		//				float palpha = std::acos(Vector3::Dot(cameraForward, direction)) / (cameraForward.magnitude() * direction.magnitude());

		//				float pC = Vector3::Distance(originWorldTransform.Position, m_EditorCamera.GetPosition());

		//				float pA = pC * std::sin(palpha);
		//				float pB = std::sqrt(std::pow(pC, 2.0f) - std::pow(pA, 2.0f));

		//				Vector3 mvPoint = m_EditorCamera.ScreenToWorldPoint(x, y, pB);
		//				auto forw = originWorldTransform.Forward();
		//				auto dir = (originWorldTransform.Position - mvPoint).normalize();
		//				//float C = Vector3::Distance(mvPoint, originWorldTransform.Position);

		//				float alpha = std::acos(Vector3::Dot(forw, dir)) / (forw.magnitude() * dir.magnitude());

		//				float C = Vector3::Distance(originWorldTransform.Position, mvPoint);

		//				float a1 = C * std::sin(alpha);
		//				//float length = Vector3::Distance(originWorldTransform.Position, mvPoint * std::cos(alpha));
		//				//float length = std::cos(alpha) * Vector3::Distance(originWorldTransform.Position, mvPoint);

		//				std::cout << "Alpha: " << alpha << ", Cos: " << std::cos(alpha) << ", Length: " << a1 << std::endl;

		//				tool_lines.emplace(Line{ originTransform.Right() * 1000.0f + originTransform.Position, originTransform.Right() * -1000.0f + originTransform.Position, Color::Red, 0.5f, -1, GizmosRenderer::LineMode::Solid });

		//				transform.SetLocalPosition(originTransform.Position + originTransform.Right() * a1 * (dir.x > 0.0f ? -1.0f : 1.0f));

		//				// DrawAngle mvPoint and originWorldTransform.Position
		//				// GizmosRenderer::DrawAngle(originWorldTransform.Position, mvPoint, dir, 5.0f, Color::Red, Matrix4x4::Identity());

		//				tool_lines.emplace(Line{ originWorldTransform.Position, originWorldTransform.Position + forw * a1, Color::Green, 0.5f, -1, GizmosRenderer::LineMode::Solid });
		//				tool_lines.emplace(Line{ originWorldTransform.Position, originWorldTransform.Position + originWorldTransform.Right() * a1, Color::Green, 0.5f, -1, GizmosRenderer::LineMode::Solid });
		//				tool_lines.emplace(Line{ originWorldTransform.Position, originWorldTransform.Position + dir * C, Color::Red, 0.5f, -1, GizmosRenderer::LineMode::Solid });
		//				tool_lines.emplace(Line{ originWorldTransform.Position, mvPoint, Color::Yellow, 0.5f, -1, GizmosRenderer::LineMode::Rounded_Dash_Dot_Dot });
		//				tool_lines.emplace(Line{ originWorldTransform.Position + originWorldTransform.Right() * a1 * (alpha > 0.0f ? -1.0f : 1.0f), mvPoint, Color::Green, 0.5f, -1, GizmosRenderer::LineMode::Dash_Dot });

		//				/*auto cameraForward = m_EditorCamera.GetForward();
		//				auto direction = (originWorldTransform.Position - m_EditorCamera.GetPosition()).normalize();

		//				float palpha = std::acos(Vector3::Dot(cameraForward, direction)) / (cameraForward.magnitude() * direction.magnitude());
		//				float pC = Vector3::Distance(originWorldTransform.Position, m_EditorCamera.GetPosition());

		//				float pA = pC * std::sin(palpha);
		//				float pB = std::sqrt(std::pow(pC, 2.0f) - std::pow(pA, 2.0f));

		//				Vector3 mvPoint = m_EditorCamera.ScreenToWorldPoint(x, y, pB);
		//				auto forw = originWorldTransform.Forward();
		//				auto dir = (originWorldTransform.Position - mvPoint).normalize();
		//				float C = Vector3::Distance(mvPoint, originWorldTransform.Position);

		//				float alpha = std::acos(Vector3::Dot(forw, dir)) / (forw.magnitude() * dir.magnitude());


		//				float A = C * std::sin(alpha);
		//				float B = std::sqrt(std::pow(C, 2.0f) - std::pow(A, 2.0f));

		//				std::cout << "Alpha: " << alpha << ", A: " << A << ", B: " << B << ", C: " << C << std::endl;

		//				tool_lines.emplace(Line{ originTransform.Right() * 1000.0f + originTransform.Position, originTransform.Right() * -1000.0f + originTransform.Position, Color::Red, 0.5f, -1, GizmosRenderer::LineMode::Solid });

		//				transform.SetLocalPosition(originTransform.Position + originTransform.Right() * A * (dir.x > 0.0f ? -1.0f : 1.0f));*/
		//			}
		//			break;
		//		case GizmosRenderer::Tool::MoveZ:
		//			{
		//				auto cameraForward = m_EditorCamera.GetForward();
		//				auto direction = (originWorldTransform.Position - m_EditorCamera.GetPosition()).normalize();

		//				float palpha = std::acos(Vector3::Dot(cameraForward, direction)) / (cameraForward.magnitude() * direction.magnitude());
		//				float pC = Vector3::Distance(originWorldTransform.Position, m_EditorCamera.GetPosition());

		//				float pA = pC * std::sin(palpha);
		//				float pB = std::sqrt(std::pow(pC, 2.0f) - std::pow(pA, 2.0f));

		//				Vector3 mvPoint = m_EditorCamera.ScreenToWorldPoint(x, y, pB);
		//				auto forw = originWorldTransform.Forward();
		//				auto dir = (originWorldTransform.Position - mvPoint).normalize();
		//				float C = Vector3::Distance(mvPoint, originWorldTransform.Position);

		//				float alpha = std::acos(Vector3::Dot(forw, dir)) / (forw.magnitude() * dir.magnitude());


		//				float A = C * std::sin(alpha);
		//				float B = std::sqrt(std::pow(C, 2.0f) - std::pow(A, 2.0f));

		//				std::cout << "Alpha: " << alpha << ", A: " << A << ", B: " << B << ", C: " << C << std::endl;

		//				tool_lines.emplace(Line{ originTransform.Forward() * 1000.0f + originTransform.Position, originTransform.Forward() * -1000.0f + originTransform.Position, Color::Green, 0.5f, -1, GizmosRenderer::LineMode::Solid });

		//				transform.SetLocalPosition(originTransform.Position - originTransform.Forward() * A * (dir.x > 0.0f ? -1.0f : 1.0f));
		//			}
		//			break;
		//		case GizmosRenderer::Tool::MoveY:
		//			{
		//				auto cameraForward = m_EditorCamera.GetForward();
		//				auto direction = (originWorldTransform.Position - m_EditorCamera.GetPosition()).normalize();

		//				float palpha = std::acos(Vector3::Dot(cameraForward, direction)) / (cameraForward.magnitude() * direction.magnitude());
		//				float pC = Vector3::Distance(originWorldTransform.Position, m_EditorCamera.GetPosition());

		//				float pA = pC * std::sin(palpha);
		//				float pB = std::sqrt(std::pow(pC, 2.0f) - std::pow(pA, 2.0f));

		//				Vector3 mvPoint = m_EditorCamera.ScreenToWorldPoint(x, y, pB);
		//				auto forw = originWorldTransform.Forward();
		//				auto dir = (originWorldTransform.Position - mvPoint).normalize();
		//				float C = Vector3::Distance(mvPoint, originWorldTransform.Position);

		//				float alpha = std::acos(Vector3::Dot(forw, dir)) / (forw.magnitude() * dir.magnitude());


		//				float A = C * std::sin(alpha);
		//				float B = std::sqrt(std::pow(C, 2.0f) - std::pow(A, 2.0f));

		//				std::cout << "Alpha: " << alpha << ", A: " << A << ", B: " << B << ", C: " << C << std::endl;

		//				tool_lines.emplace(Line{ originTransform.Up() * 1000.0f + originTransform.Position, originTransform.Up() * -1000.0f + originTransform.Position, Color::Blue, 0.5f, -1, GizmosRenderer::LineMode::Solid });

		//				transform.SetLocalPosition(originTransform.Position + originTransform.Up() * A * (dir.x > 0.0f ? -1.0f : 1.0f));
		//			}
		//			break;
		//		case GizmosRenderer::Tool::MoveXYZ:
		//			{
		//				auto cameraForward = m_EditorCamera.GetForward();
		//				auto direction = (originWorldTransform.Position - m_EditorCamera.GetPosition()).normalize();

		//				float alpha = std::acos(Vector3::Dot(cameraForward, direction)) / (cameraForward.magnitude() * direction.magnitude());
		//				float C = Vector3::Distance(originWorldTransform.Position, m_EditorCamera.GetPosition());

		//				float A = C * std::sin(alpha);
		//				float B = std::sqrt(std::pow(C, 2.0f) - std::pow(A, 2.0f));

		//				auto newLocation = m_EditorCamera.ScreenToWorldPoint(x, y, B);
		//				transform.SetLocalPosition(newLocation);
		//				tool_lines.emplace(Line{ originTransform.Position, newLocation, Color::BlanchedAlmond, 0.8f, -1, GizmosRenderer::LineMode::Rounded_Dashed });
		//				break;
		//			}
		//		default: break;
		//		}

		//		if (InputManager::IsKeyPressed(HL_KEY_ESCAPE)) { current_tool = GizmosRenderer::Tool::None; transform.GetTransformComponent() = originTransform; }

		//	}

		//	//static bool notf = true;

		//	//if (!SceneRegistry::GetCurrentScene().lock()->IsPrimaryCameraSet()) {
		//	//	if (notf) {
		//	//		auto pos = ImGui::GetWindowPos();
		//	//		auto Size = ImGui::GetWindowSize();
		//	//		ImGui::SetNextWindowFocus();
		//	//		ImGui::SetNextWindowBgAlpha(1.0f);
		//	//		ImGui::SetWindowFontScale(2);
		//	//		ImGui::SetNextWindowPos(ImVec2(pos.x + (Size.x / 2.0f) - 200.0f, pos.y + (Size.y / 2.0f) - 100.0f));
		//	//		ImGui::BeginChild("MissingCameraText", ImVec2(400, 200), true);
		//	//		//ImGui::Button("Test");
		//	//		//ImGui::SetCursorPos(ImVec2(0, 0));
		//	//		ImGui::Text("No CameraComponent Present for rendering!");
		//	//		if (ImGui::Button("Close")) {
		//	//			notf = false;
		//	//		}
		//	//		ImGui::EndChild();
		//	//	}
		//	//}
		//	//else notf = true;

		//	if (ImGui::IsWindowHovered())
		//	{
		//		static ImVec2 origin;
		//		static bool mActive = false;

		//		if (ImGui::IsMouseClicked(ImGuiMouseButton_Right))
		//		{
		//			ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
		//			origin = ImGui::GetMousePos();
		//			mActive = true;
		//		}

		//		if (mActive)
		//		{
		//			//std::cout << "Mouse Is Held" << std::endl;
		//			ImVec2 current = ImGui::GetMousePos();

		//			auto difference = Vector2(origin.x, origin.y) - Vector2(current.x, current.y);
		//			auto normalizedCordinates = -(Vector2(origin.y, origin.x) - Vector2(current.y, current.x)).normalize();
		//			
		//			m_EditorCamera.HandleMovement(-(Vector2(origin.y, origin.x) - Vector2(current.y, current.x)).normalize());

		//			SetCursorPos(origin.x, origin.y);
		//		}

		//		if (ImGui::IsMouseReleased(ImGuiMouseButton_Right)) {
		//			mActive = false;
		//			ImGui::SetMouseCursor(ImGuiMouseCursor_Arrow);
		//		}
		//	}

		//	if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) && ImGui::IsKeyDown(ImGuiKey_R)) {
		//		m_EditorCamera.Reset();
		//	}
		//	m_isVisible = true;
		//		
		//}
		//else m_isVisible = false;

		//ImGui::End();
		//ImGui::PopStyleVar();
	}
}
