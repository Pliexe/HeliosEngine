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

	const Vector3 EditorCamera::GetForward()
	{
		return m_TransformComponent.Forward();
	}

	const Vector3 EditorCamera::GetUp()
	{
		return m_TransformComponent.Up();
	}

	const Vector3 EditorCamera::GetRight()
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

	Vector3 EditorCamera::ScreenToWorldPoint(const Vector2& screenPoint, float z)
	{
		return Camera::ScreenToWorldPoint(screenPoint, z, GetViewMatrix());
	}

	Vector3 EditorCamera::ScreenToWorldPoint(float x, float y, float z)
	{
		return Camera::ScreenToWorldPoint(x, y, z, GetViewMatrix());
	}

	Vector3 EditorCamera::ScreenToWorldPoint(const Vector3& screenPoint)
	{
		return Camera::ScreenToWorldPoint(screenPoint, GetViewMatrix());
	}

	Vector2 EditorCamera::WorldToScreenPoint(const Vector3& worldPoint)
	{
		return Camera::WorldToScreenPoint(worldPoint, GetViewMatrix());
	}

	Vector2 EditorCamera::WorldToScreenPoint(float x, float y, float z)
	{
		return Camera::WorldToScreenPoint(x, y, z, GetViewMatrix());
	}

	void EditorCamera::HandleMovement(Vector2 direction)
    {
		auto rotationEuler = m_TransformComponent.Rotation.euler();

		float pitchDelta = direction.y * m_Sensitivity * Time::DeltaTime();
		float yawDelta   = direction.x * m_Sensitivity * Time::DeltaTime();

		rotationEuler.x = std::clamp(rotationEuler.x + pitchDelta, -89.0f, 89.0f);
		rotationEuler.y += yawDelta;

		m_TransformComponent.Rotation = Quaternion::FromEuler(rotationEuler);
		m_TransformComponent.Rotation.normalize();



		float speed = (InputManager::IsKeyPressed(HL_KEY_SHIFT) ? m_SpeedMultiplier : 1.0f) * m_Speed;

		std::cout << "Forward: " << m_TransformComponent.Forward().to_string() << std::endl;
		std::cout << "Right:   " << m_TransformComponent.Right().to_string() << std::endl;
		std::cout << "Up:      " << m_TransformComponent.Up().to_string() << std::endl;

		Vector3 movement;

		if (InputManager::IsKeyDown(HL_KEY_W))
			movement += m_TransformComponent.Forward() * Time::DeltaTime() * speed;

		if (InputManager::IsKeyDown(HL_KEY_S))
			movement -= m_TransformComponent.Forward() * Time::DeltaTime() * speed;

		if (InputManager::IsKeyDown(HL_KEY_D))
			movement += m_TransformComponent.Right() * Time::DeltaTime() * speed;

		if (InputManager::IsKeyDown(HL_KEY_A))
			movement -= m_TransformComponent.Right() * Time::DeltaTime() * speed;

		if (InputManager::IsKeyDown(HL_KEY_E))
			movement += m_TransformComponent.Up() * Time::DeltaTime() * speed;

		if (InputManager::IsKeyDown(HL_KEY_Q))
			movement -= m_TransformComponent.Up() * Time::DeltaTime() * speed;

		m_TransformComponent.Translate(movement);
    }

	void EditorCamera::Reset()
	{
		m_TransformComponent = TransformComponent();
	}
}
