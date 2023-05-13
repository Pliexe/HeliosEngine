#pragma once

#include "Helios/Core/Base.h"
#include "Components.h"
#include "Helios/Translation/Matrix.h"

namespace Helios {
	class HELIOS_API SceneCamera
	{
	public:

		SceneCamera() = default;
		SceneCamera(TransformComponent transform, float fov, float nearClip, float farClip);
		SceneCamera(TransformComponent transform, CameraComponent camera);
		SceneCamera(CameraComponent);
		SceneCamera(const SceneCamera&) = default;

		static Vector3 ScreenToWorldCoordinates(const Vector3& screenCoordinates, const TransformComponent& transform, const CameraComponent& camera, Size size =
			DepricatedGraphics::GetCurrentSize()) {
			return ScreenToWorldCoordinates(screenCoordinates.x, screenCoordinates.y, screenCoordinates.z, transform, camera, size);
		}

		static Vector3 ScreenToWorldCoordinates(float x, float y, float depth, const TransformComponent& transform, const CameraComponent& camera, Size size =
			                                   DepricatedGraphics::GetCurrentSize());

		Vector3 ScreenToWorldCoordinates(const Vector3& screenCoordinates, Size size =
			DepricatedGraphics::GetCurrentSize()) {
			return ScreenToWorldCoordinates(screenCoordinates, m_Transform, m_Camera, size);
		}

		Vector3 ScreenToWorldCoordinates(float x, float y, float depth = 5.0f, Size size =
			DepricatedGraphics::GetCurrentSize()) {
			return ScreenToWorldCoordinates(x, y, depth, m_Transform, m_Camera, size);
		}
		
		static inline Matrix4x4 GetViewProjection(const TransformComponent& transform, const CameraComponent& camera, Size size = DepricatedGraphics::GetCurrentSize());
		static Matrix4x4 GetViewMatrix(const TransformComponent& transform);
		static Matrix4x4 GetProjectionMatrix(const CameraComponent& camera, Size size);
		static inline Matrix4x4 GetViewProjection(std::tuple<const TransformComponent&, const CameraComponent&> camObj) { auto [transform, camera] = camObj; return GetViewProjection(transform, camera); }
		inline Matrix4x4 GetViewProjection() { return GetViewProjection(m_Transform, m_Camera); }
		
		CameraComponent& GetCamera() { return m_Camera; }
		const CameraComponent& GetCamera() const { return m_Camera; }
		
		TransformComponent& GetTransform() { return m_Transform; }
		const TransformComponent& GetTransform() const { return m_Transform; }

		void HandleControls(const Vector2 direction);
		inline void Reset();
		
	private:
		
		CameraComponent m_Camera;
		TransformComponent m_Transform;
		float sensitivity = 150.0f;
	};
}