#include "EditorCamera.h"

#include "Helios/Core/Time.h"
#include "Helios/Input/InputManager.h"
#include "Helios/Input/KeyCodes.h"

namespace Helios
{
	EditorCamera::EditorCamera() : Camera() { }

	const Vector3& EditorCamera::GetPosition() const
	{
		return m_TransformComponent.Position;
	}

	const Vector3 EditorCamera::GetRotation()
	{
		return m_TransformComponent.Rotation.euler();
	}

	const Quaternion& EditorCamera::GetOrientation() const
	{
		return m_TransformComponent.Rotation;
	}

	const Vector3& EditorCamera::GetForward()
	{
		return m_TransformComponent.Forward();
	}

	const Vector3& EditorCamera::GetUp()
	{
		return m_TransformComponent.Up();
	}

	const Vector3& EditorCamera::GetRight()
	{
		return m_TransformComponent.Right();
	}

	void EditorCamera::SetPosition(const Vector3& position)
	{
		m_TransformComponent.Position = position;
	}

	void EditorCamera::SetRotation(const Vector3& rotation)
	{
		m_TransformComponent.Rotation = Quaternion::FromEuler(rotation);
	}

	void EditorCamera::SetOrientation(const Quaternion& quaternion)
	{
		m_TransformComponent.Rotation = quaternion;
	}

	Matrix4x4 EditorCamera::GetViewMatrix()
	{
		return m_TransformComponent.GetModelMatrix();
	}

	Matrix4x4 EditorCamera::GetViewMatrix() const
	{
		return m_TransformComponent.GetModelMatrix();
	}

	const Matrix4x4 EditorCamera::GetViewProjection()
	{
		return GetProjectionMatrix() * Matrix4x4::Inverse(GetViewMatrix());
	}

	const Matrix4x4 EditorCamera::GetViewProjection() const
	{
		return GetProjectionMatrix() * Matrix4x4::Inverse(GetViewMatrix());
	}

	Vector3 EditorCamera::ScreenToWorldPoint(const Vector2& screenPoint, float depth)
	{
		return Camera::ScreenToWorldPoint(screenPoint, depth, GetViewMatrix());
	}

	Vector3 EditorCamera::ScreenToWorldPoint(float x, float y, float depth)
	{
		return Camera::ScreenToWorldPoint(x, y, depth, GetViewMatrix());
	}

	Vector3 EditorCamera::ScreenToWorldPoint(const Vector3& screenPoint)
	{
		return Camera::ScreenToWorldPoint(screenPoint, GetViewMatrix());
	}

	void EditorCamera::HandleMovement(Vector2 direction)
    {
		m_TransformComponent.Rotation = Quaternion::FromEuler(m_TransformComponent.Rotation.euler() + direction * Time::deltaTime() * m_Sensitivity);

		float speed = (InputManager::IsKeyPressed(HL_KEY_SHIFT) ? m_SpeedMultiplier : 1.0f) * m_Speed;

		Vector3 movement;

		if (InputManager::IsKeyDown(HL_KEY_W))
			movement += m_TransformComponent.Forward() * Time::deltaTime() * speed;

		if (InputManager::IsKeyDown(HL_KEY_S))
			movement -= m_TransformComponent.Forward() * Time::deltaTime() * speed;

		if (InputManager::IsKeyDown(HL_KEY_D))
			movement += m_TransformComponent.Right() * Time::deltaTime() * speed;

		if (InputManager::IsKeyDown(HL_KEY_A))
			movement -= m_TransformComponent.Right() * Time::deltaTime() * speed;

		if (InputManager::IsKeyDown(HL_KEY_E))
			movement += m_TransformComponent.Up() * Time::deltaTime() * speed;

		if (InputManager::IsKeyDown(HL_KEY_Q))
			movement -= m_TransformComponent.Up() * Time::deltaTime() * speed;

		m_TransformComponent.Translate(movement);
    }

	void EditorCamera::Reset()
	{
		m_TransformComponent = TransformComponent();
	}
}
