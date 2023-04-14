#include "Camera.h"
#include "Helios/Core/Asserts.h"

namespace Helios
{
	Camera::Camera(uint32_t viewport_width, uint32_t viewport_height) : m_ViewportSize(viewport_width, viewport_height) { InvalidateProjection(); }

	void Camera::SetOrthographic(float size, float nearClip, float farClip)
	{
        m_Size = size;
        m_NearClip = nearClip;
        m_FarClip = farClip;
        m_Type = Orthographic;
        InvalidateProjection();
	}

	void Camera::SetPerspective(float fov, float nearClip, float farClip)
	{
        m_Fov = fov;
        m_NearClip = nearClip;
        m_FarClip = farClip;
        m_Type = Perspective;
        InvalidateProjection();
	}

	void Camera::SetProjection(const Matrix4x4& projection)
	{
        m_ProjectionMatrix = projection;
        InvalidateProjection();
	}

	void Camera::SetViewportSize(uint32_t viewport_width, uint32_t viewport_height)
	{
        HL_ASSERT_EXCEPTION(viewport_width > 0 && viewport_height > 0, "Viewport Size cannot be zero!");
		m_ViewportSize = Size(viewport_width, viewport_height);
		InvalidateProjection();
	}

	void Camera::SetViewportSize(Size viewport_size)
	{
        HL_ASSERT_EXCEPTION(viewport_size.width > 0 && viewport_size.height > 0, "Viewport Size cannot be zero!");
		m_ViewportSize = viewport_size;
		InvalidateProjection();
	}

	void Camera::SetNearClip(float nearClip) { m_NearClip = nearClip; }
	void Camera::SetFarClip(float farClip) { m_FarClip = farClip; }
	void Camera::SetFov(float fov) { m_Fov = fov; }
	void Camera::SetSize(float size) { m_Size = size; }

	const Matrix4x4& Camera::GetProjectionMatrix() const
	{
        return m_ProjectionMatrix;
	}

	Vector3 Camera::ScreenToWorldPoint(const Vector3& screenPoint, const Matrix4x4& viewMatrix) const
	{
		return ScreenToWorldPoint(screenPoint.x, screenPoint.y, screenPoint.z, viewMatrix);
	}

	Vector3 Camera::ScreenToWorldPoint(const Vector2& screenPoint, float depth, const Matrix4x4& viewMatrix) const
	{
		return ScreenToWorldPoint(screenPoint.x, screenPoint.y, depth, viewMatrix);
	}

	Vector3 Camera::ScreenToWorldPoint(float x, float y, float depth, const Matrix4x4& viewMatrix) const
	{
		// Get ray position in world space from screen point (x, y) and scale it by depth

		float ndcX = (2.0f * x) / static_cast<float>(m_ViewportSize.width) - 1.0f;
		float ndcY = 1.0f - (2.0f * y) / static_cast<float>(m_ViewportSize.height);

		Vector4 rayClip = Vector4(ndcX, ndcY, -1.0f, 1.0f);
		Vector4 rayEye = Matrix4x4::Inverse(m_ProjectionMatrix) * rayClip;
		rayEye *= depth; // Add the depth
		rayEye = Vector4(rayEye.x, rayEye.y, depth, 1.0f);
		Vector4 rayWorld = viewMatrix * rayEye;

		return Vector3(rayWorld.x, rayWorld.y, rayWorld.z);		
	}

	void Helios::Camera::InvalidateProjection()
	{
		switch (m_Type)
		{
		case Perspective:
			m_ProjectionMatrix = Matrix4x4::PerspectiveLH(m_Fov, (float)m_ViewportSize.width / (float)m_ViewportSize.height, m_NearClip, m_FarClip);
			break;
		case Orthographic:
			m_ProjectionMatrix = Matrix4x4::OrthographicLH(m_Size, (float)m_ViewportSize.width / (float)m_ViewportSize.height, m_NearClip, m_FarClip);
			break;
		}
	}

	inline Camera::PerspectiveData Camera::GetPerspective() const
    {
        return { m_Fov, m_NearClip, m_FarClip };
    }

    Camera::OrthographicData Camera::GetOrthographic() const
    {
        return { m_Size, m_NearClip, m_FarClip };
    }
}
