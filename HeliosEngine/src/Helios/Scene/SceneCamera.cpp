#include "SceneCamera.h"
#include "Helios/Core/Time.h"
#include "imgui.h"

#include "Helios/Input/InputManager.h"
#include "Helios/Input/KeyCodes.h"

namespace Helios {
	SceneCamera::SceneCamera(Components::Transform transform, float fov, float nearClip, float farClip)
	{
		m_Transform = transform;
		m_Camera.fov = fov;
		m_Camera.near_z = nearClip;
		m_Camera.far_z = farClip;
	}
	SceneCamera::SceneCamera(Components::Transform transform, Components::Camera camera)
	{
		m_Transform = transform;
		m_Camera = camera;
	}
	SceneCamera::SceneCamera(Components::Camera camera)
	{
		m_Camera = camera;
	}
	inline Matrix4x4 SceneCamera::GetProjection(const Components::Transform& transform, const Components::Camera& camera)
	{
		auto size = Graphics::GetCurrentSize();

		return Matrix4x4::Transpose(
			Matrix4x4::Translation(-((Vector3)transform.position)) *
			Matrix4x4::RotationColumn(transform.rotation) *
			(camera.ortographic ? (
				Matrix4x4::OrthographicLH(camera.size, camera.size * ((float)size.y / (float)size.x), camera.near_z, camera.far_z)
			) : (
				Matrix4x4::PerspectiveLH(camera.fov * 3.14f / 180.0f, ((float)size.x / (float)size.y), camera.near_z, camera.far_z)
			))
		);
	}
	void SceneCamera::HandleControls(const Vector2 direction)
	{
		//m_Transform.rotation = Quanterion::Euler(m_Transform.rotation.euler() + direction * Time::deltaTime() * sensitivity);

		m_Transform.Rotate(direction * Time::deltaTime() * sensitivity);

		float speed = InputManager::IsKeyPressed(HL_KEY_SHIFT) ? 20.0f : 10.0f;

		if (InputManager::IsKeyPressedDown(HL_KEY_W))
			m_Transform.position += m_Transform.forward() * Time::deltaTime() * speed;

		if (InputManager::IsKeyPressedDown(HL_KEY_S))
			m_Transform.position -= m_Transform.forward() * Time::deltaTime() * speed;

		if (InputManager::IsKeyPressedDown(HL_KEY_D))
			m_Transform.position += m_Transform.right() * Time::deltaTime() * speed;

		if (InputManager::IsKeyPressedDown(HL_KEY_A))
			m_Transform.position -= m_Transform.right() * Time::deltaTime() * speed;
	}
	inline void SceneCamera::Reset()
	{
		m_Transform.position = Vector3::Zero();
		m_Transform.rotation = Quanterion::Identity();
	}
}