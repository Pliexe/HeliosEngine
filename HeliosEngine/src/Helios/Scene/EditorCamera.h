#pragma once
#include "Camera.h"
#include "Components/Transforms.h"

namespace Helios
{
	class HELIOS_API EditorCamera : public Camera
	{
	public:
		EditorCamera();
		const Vector3& GetPosition() const;
		const Vector3 GetRotation();
		const Quaternion& GetOrientation() const;
		const Vector3 GetForward();
		const Vector3 GetUp();
		const Vector3 GetRight();
		void SetPosition(const Vector3& position);
		void SetRotation(const Vector3& rotation);
		void SetOrientation(const Quaternion& quaternion);

		Matrix4x4 GetViewMatrix();
		Matrix4x4 GetViewMatrix() const;
		const Matrix4x4 GetViewProjection();
		const Matrix4x4 GetViewProjection() const;

		TransformComponent GetTransformComponent() const { return m_TransformComponent; }

		Vector3 ScreenToWorldPoint(const Vector2& screenPoint, float z);
		Vector3 ScreenToWorldPoint(float x, float y, float z);
		Vector3 ScreenToWorldPoint(const Vector3& screenPoint);

		Vector2 WorldToScreenPoint(const Vector3& worldPoint);
		Vector2 WorldToScreenPoint(float x, float y, float z);

		void HandleMovement(Vector2 direction);
		void Reset();

	private:
		TransformComponent m_TransformComponent;
		float m_Speed = 10.0f;
		float m_SpeedMultiplier = 2.0f;
		float m_Sensitivity = 50.0f;
	};
}
