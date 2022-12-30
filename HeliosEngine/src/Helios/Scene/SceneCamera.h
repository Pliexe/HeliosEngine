#pragma once

#include "Helios/Core/Base.h"
#include "Components.h"
#include "Helios/Translation/Matrix.h"

namespace Helios {
	class HELIOS_API SceneCamera
	{
	public:

		SceneCamera() = default;
		SceneCamera(Components::Transform transform, float fov, float nearClip, float farClip);
		SceneCamera(Components::Transform transform, Components::Camera camera);
		SceneCamera(Components::Camera);
		SceneCamera(const SceneCamera&) = default;

		static inline Matrix4x4 GetProjection(const Components::Transform& transform, const Components::Camera& camera);
		inline Matrix4x4 GetProjection() const { return GetProjection(m_Transform, m_Camera); }
		
		Components::Camera& GetCamera() { return m_Camera; }
		const Components::Camera& GetCamera() const { return m_Camera; }
		
		Components::Transform& GetTransform() { return m_Transform; }
		const Components::Transform& GetTransform() const { return m_Transform; }

		void HandleControls(const Vector2 direction);
		inline void Reset();
		
	private:
		
		Components::Camera m_Camera;
		Components::Transform m_Transform;
		float sensitivity = 100.0f;
	};
}