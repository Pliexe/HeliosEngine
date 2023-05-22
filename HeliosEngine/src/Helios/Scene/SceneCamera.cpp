#include "SceneCamera.h"

#include "Entity.h"
#include "Helios/Core/Time.h"
#include "imgui.h"

#include "Helios/Input/InputManager.h"
#include "Helios/Input/KeyCodes.h"

namespace Helios {
	SceneCamera::SceneCamera(TransformComponent transform, float fov, float nearClip, float farClip)
	{
		m_Transform = transform;
		m_Camera.fov = fov;
		m_Camera.near_z = nearClip;
		m_Camera.far_z = farClip;
	}
	SceneCamera::SceneCamera(TransformComponent transform, CameraComponent camera)
	{
		m_Transform = transform;
		m_Camera = camera;
	}
	SceneCamera::SceneCamera(CameraComponent camera)
	{
		m_Camera = camera;
	}

	Vector3 SceneCamera::ScreenToWorldCoordinates(float x, float y, float depth, const TransformComponent& transform,
		const CameraComponent& camera, Size screen_size)
	{
		Vector4 screenCoords = {
			(x / screen_size.width) * 2.0f - 1.0f,
			(y / screen_size.height) * -2.0f + 1.0f,
			depth, 1.0f
		};

		Matrix4x4 viewMatrix = GetViewMatrix(transform);
		Matrix4x4 projectionMatrix = GetProjectionMatrix(camera, screen_size);

		Matrix4x4 invViewProj = Matrix4x4::Inverse(viewMatrix);
		//Matrix4x4 invViewProj = Matrix4x4::Inverse(projectionMatrix * viewMatrix);
		Vector4 worldCoords = invViewProj * screenCoords;
		return Vector3(worldCoords.x, worldCoords.y, worldCoords.z);
	}

	/*
	Matrix4x4 SceneCamera::GetViewMatrix(const TransformComponent& transform)
	{
		Matrix4x4 viewMatrix = Matrix4x4::Identity();
		viewMatrix = Matrix4x4::Translate(viewMatrix, transform.position);
		viewMatrix = Matrix4x4::Rotate(viewMatrix, transform.rotation);
		return viewMatrix;
	}
	Matrix4x4 SceneCamera::GetProjectionMatrix(const CameraComponent& camera, Size screen_size)
	{
		return Matrix4x4::Perspective(camera.fov, screen_size.width / screen_size.height, camera.near_z, camera.far_z);
	}

	void SceneCamera::HandleControls(const Vector2 direction)
	{
		float delta = Time::GetDeltaTime();
		m_Transform.Rotate(Vector3(0.0f, 1.0f, 0.0f), direction.x * delta * sensitivity);
		m_Transform.Rotate(Vector3(1.0f, 0.0f, 0.0f), direction.y * delta * sensitivity);
	}

	void SceneCamera::Reset()
	{
		m_Transform.position = Vector3(0.0f, 0.0f, 0.0f);
		m_Transform.rotation = Vector3(0.0f, 0.0f, 0.0f);
	}
}
	}*/
	
	inline Matrix4x4 SceneCamera::GetViewProjection(const TransformComponent& transform, const CameraComponent& camera, Size size)
	{
		return GetProjectionMatrix(camera, size) * GetViewMatrix(transform);
	}

	Matrix4x4 SceneCamera::GetViewMatrix(const TransformComponent& transform)
	{
		return Matrix4x4::RotationColumn(transform.Rotation) * Matrix4x4::TranslationColumn(-(Vector3)transform.Position);
	}

	Matrix4x4 SceneCamera::GetProjectionMatrix(const CameraComponent& camera, Size size = DepricatedGraphics::GetCurrentSize())
	{
		return camera.ortographic ? (
			Matrix4x4::OrthographicLH(camera.size, camera.size * ((float)size.height / (float)size.width), camera.near_z, camera.far_z)
		) : (
			Matrix4x4::PerspectiveLH(camera.fov * 3.14f / 180.0f, ((float)size.width / (float)size.height), camera.near_z, camera.far_z)
		);
	}

	void SceneCamera::HandleControls(const Vector2 direction)
	{
		m_Transform.Rotation = Quaternion::FromEuler(m_Transform.Rotation.euler() + direction * Time::deltaTime() * sensitivity);

		float speed = InputManager::IsKeyPressed(HL_KEY_SHIFT) ? 20.0f : 10.0f;

		Vector3 move = Vector3::Zero();

		if (InputManager::IsKeyDown(HL_KEY_W))
			move += m_Transform.Forward() * Time::deltaTime() * speed;

		if (InputManager::IsKeyDown(HL_KEY_S))
			move -= m_Transform.Forward() * Time::deltaTime() * speed;

		if (InputManager::IsKeyDown(HL_KEY_D))
			move += m_Transform.Right() * Time::deltaTime() * speed;

		if (InputManager::IsKeyDown(HL_KEY_A))
			move -= m_Transform.Right() * Time::deltaTime() * speed;

		if (InputManager::IsKeyDown(HL_KEY_E))
			move += m_Transform.Up() * Time::deltaTime() * speed;

		if (InputManager::IsKeyDown(HL_KEY_Q))
			move -= m_Transform.Up() * Time::deltaTime() * speed;
				
		m_Transform.Translate(move);
	}

	inline void SceneCamera::Reset()
	{
		m_Transform.Position = Vector3::Zero();
		m_Transform.Rotation = Quaternion::Identity();
	}
}