#pragma once

#include "Helios/Core/Base.h"
#include "Helios/Math/Matrix.h"
#include "Components/Basic.h"
#include "Components/Transforms.h"

namespace Helios
{
    class HELIOS_API Camera
    {
    public:
        enum Type { Orthographic, Perspective };
        struct PerspectiveData { float fov;  float near_z; float far_z; };
        struct OrthographicData { float size; float near_z; float far_z; };

        Camera() { InvalidateProjection(); }
        Camera(const Camera&) = default;
        Camera(Camera&&) = default;
        Camera(uint32_t viewport_width, uint32_t viewport_height);
        Camera& operator=(const Camera&) = default;
        Camera& operator=(Camera&&) = default;
        virtual ~Camera() = default;

        void SetOrthographic(float size, float nearClip, float farClip);
        void SetPerspective(float fov, float nearClip, float farClip);
        void SetProjection(const Matrix4x4& projection);
        void SetViewportSize(uint32_t viewport_width, uint32_t viewport_height);
        void SetViewportSize(Size viewport_size);

        void SetNearClip(float nearClip);
        void SetFarClip(float farClip);
        void SetFov(float fov);
        void SetSize(float size);

        const Matrix4x4& GetProjectionMatrix() const;

        Vector3 ScreenToWorldPoint(const Vector3& screenPoint, const Matrix4x4& viewMatrix) const;
        Vector3 ScreenToWorldPoint(const Vector2& screenPoint, float z, const Matrix4x4& viewMatrix) const;
        Vector3 ScreenToWorldPoint(float x, float y, float z, const Matrix4x4& viewMatrix) const;
        Vector2 WorldToScreenPoint(const Vector3& worldPoint, const Matrix4x4& viewMatrix) const;

        Vector2 WorldToScreenPoint(float x, float y, float z, const Matrix4x4& viewMatrix) const;
        void InvalidateProjection();

        PerspectiveData GetPerspective() const;
        OrthographicData GetOrthographic() const;

        float GetNearClip() { return m_NearClip; }
        float GetFarClip() { return m_FarClip; }
        float GetSize() { return m_Size; }
        float GetFov() { return m_Fov; }
        Type GetType() { return m_Type; }

		static Matrix4x4 GetViewProjection(const Matrix4x4& viewMatrix, const Matrix4x4& projectionMatrix) { return projectionMatrix * Matrix4x4::Inverse(viewMatrix); }
        static Matrix4x4 GetViewProjection(const TransformComponent& transform, const CameraComponent& camera, const Size& viewport)
        {
            Matrix4x4 projectionMatrix = camera.ortographic ?
				Matrix4x4::OrthographicLH(camera.size, (float)viewport.width / (float)viewport.height, camera.near_z, camera.far_z) :
				Matrix4x4::PerspectiveLH(camera.fov, (float)viewport.width / (float)viewport.height, camera.near_z, camera.far_z);

			return GetViewProjection(transform.GetModelMatrix(), projectionMatrix);
        }

    protected:
        Matrix4x4 m_ProjectionMatrix = Matrix4x4::Identity();
        Size m_ViewportSize;
        float m_NearClip = 0.1f;
        float m_FarClip = 1000.0f;
        float m_Size = 10.0f;
        float m_Fov = 45.0f;
        Type m_Type = Perspective;
    };
}
