#include "Camera.h"
#include "Helios/Core/Asserts.h"

namespace Helios
{
	Camera::Camera(uint32_t viewport_width, uint32_t viewport_height) : m_AspectRatio((float)viewport_width / (float)viewport_height) { InvalidateProjection(); }

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
        m_AspectRatio = (float)viewport_width / (float)viewport_height;
		InvalidateProjection();
	}

	void Camera::SetViewportSize(Size viewport_size)
	{
        HL_ASSERT_EXCEPTION(viewport_size.width() > 0 && viewport_size.height() > 0, "Viewport Size cannot be zero!");
        m_AspectRatio = (float)viewport_size.width() / (float)viewport_size.height();
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
		x = (2.0f * x) / m_ViewportSize.width() - 1.0f;
		y = 1.0f - (2.0f * y) / m_ViewportSize.height();

		Matrix4x4 viewProjection = m_ProjectionMatrix * viewMatrix;
		Matrix4x4 inverseViewProjection = Matrix4x4::Inverse(viewProjection);

		Vector4 screenSpacePoint = Vector4(x, y, depth, 1.0f);
		Vector4 worldSpacePoint = inverseViewProjection * screenSpacePoint;
		worldSpacePoint /= worldSpacePoint.w;

		return Vector3(worldSpacePoint.x, worldSpacePoint.y, worldSpacePoint.z);
	}

	void Helios::Camera::InvalidateProjection()
	{
		switch (m_Type)
		{
		case Perspective:
			m_ProjectionMatrix = Matrix4x4::PerspectiveLH(m_Fov, m_AspectRatio, m_NearClip, m_FarClip);
			break;
		case Orthographic:
			m_ProjectionMatrix = Matrix4x4::OrthographicLH(m_Size, m_AspectRatio, m_NearClip, m_FarClip);
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
