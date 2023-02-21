#pragma once

#include "Helios/Core/Base.h"

#include "Helios/Translation/Vector.h"
#include "Helios/Translation/Quanterion.h"
#include "Helios/Resources/Color.h"
#include "Helios/Resources/Mesh.h"
#include "Helios/Resources/Texture.h"
#include "Helios/Resources/Material.h"
#include "Helios/Translation/Matrix.h"

namespace Helios::Components {

	struct Relationship
	{
		entt::entity top_handle = entt::null;
		entt::entity parent_handle = entt::null;
		entt::entity first_child = entt::null;
		entt::entity prev_child = entt::null;
		entt::entity next_child = entt::null;

		Relationship() = default;
		Relationship(const Relationship&) = default;
		Relationship(entt::registry& registry, entt::entity destination, entt::entity source) { SetParent(registry, destination, source); }
		
		inline bool HasChild() { return first_child != entt::null; }
		inline bool HasParent() { return parent_handle != entt::null; }

		void SetParent(entt::registry& registry, entt::entity destination, entt::entity source)
		{
			Reset(registry);
			if (top_handle == entt::null) top_handle = source;
			parent_handle = source;
			auto& parent = registry.get<Relationship>(source);
			next_child = parent.first_child;
			parent.first_child = destination;
		}

		void Clear() {
			parent_handle = entt::null;
			first_child = entt::null;
			prev_child = entt::null;
			next_child = entt::null;
			top_handle = entt::null;
		}

		void Reset(entt::registry& registry) {
			if (parent_handle != entt::null) {
				if (prev_child == entt::null) {
					auto& parent = registry.get<Relationship>(parent_handle);
					parent.first_child = next_child;
					next_child = entt::null;
				}
				else {
					auto& prev = registry.get<Relationship>(prev_child);
					if (next_child != entt::null)
					{
						auto& next = registry.get<Relationship>(next_child);
						next.prev_child = prev_child;
						prev.next_child = next_child;
						next_child = entt::null;
						prev_child = entt::null;
					}
					else prev.next_child = entt::null;
				}
				parent_handle = entt::null;
				top_handle = entt::null;
			}
			else {
				parent_handle = entt::null;
				top_handle = entt::null;
				prev_child = entt::null;
				next_child = entt::null;
			}
		}
	};

	struct HELIOS_API DisabledObject
	{
		short a;
	};

	struct HELIOS_API DirectionalLight
	{
		Color color;
		float intensity;

		DirectionalLight() = default;
		DirectionalLight(const Color& color, float intensity) : color(color), intensity(intensity) {}
	};

	struct HELIOS_API GlobalObject { byte a; };

	struct HELIOS_API InfoComponent
	{
		std::string name;

		InfoComponent() = default;
		InfoComponent(const InfoComponent&) = default;
		InfoComponent(const std::string& name) : name(name) { }
	};

	struct HELIOS_API Transform
	{
		Vector3		localPosition = Vector3::Zero();
		Vector3		position = Vector3::Zero();
		Quanterion	localRotation = Quanterion::Identity();
		Quanterion	rotation = Quanterion::Identity();
		// Vector3	 rotationVec = { 0.0f, 0.0f, 0.0f };
		Vector3		localScale	 = { 1.0f, 1.0f, 1.0f };
		Vector3		scale	 = { 1.0f, 1.0f, 1.0f };
		bool typeSwitch = false;

		Transform() = default;
		Transform(const Transform&) = default;
		Transform(const Vector3& position) : position(position) { }
		Transform(const Vector3& position, const Vector3& rotation) : position(position), rotation(rotation) { }
		Transform(const Vector3& position, const Vector3& rotation, const Vector3& scale) : position(position), rotation(rotation), scale(scale) { }

		void Rotate(const Vector3& euler) {
			//rotation *= Quanterion::Euler(euler);
			localRotation = Quanterion::FromEuler(rotation.euler() + euler);
			rotation = localRotation;
		}
		void RotateRads(const Vector3& euler) { this->rotation = this->rotation * Quanterion::FromEulerRads(euler); }
		void Translate(const Vector3& translation) { this->localPosition += translation; this->position = this->localPosition; }
		void UpdateTransform(const Vector3& position, const Vector3& rotation, const Vector3& scale) {
			this->localPosition = position;
			this->localRotation = Quanterion::FromEuler(rotation);
			this->localScale = scale;
			this->scale = localScale;
			this->position = this->localPosition;
			this->rotation = this->localRotation;
		}
		void UpdatePosition(const Vector3& position) {
			this->localPosition = position;
			this->position = this->localPosition;
		}
		void UpdateRotation(const Quanterion& rotation) {
			this->localRotation = rotation;
			this->rotation = this->localRotation;
		}
		void UpdateRotation(const Vector3& rotation) {
			this->localRotation = Quanterion::FromEuler(rotation);
			this->rotation = this->localRotation;
		}
		void UpdateScale(const Vector3& scale) {
			this->localScale = scale;
			this->scale = this->localScale;
		}

		inline Vector3 forward() { return rotation.forward(); }
		//Vector3 forward() { return rotation * Vector3::Forward(); }
		inline Vector3 right() { return rotation * Vector3::Right(); }

		inline Matrix4x4 GetWorldProjectionColumn() const { return Matrix4x4::TranslationColumn(position) * Matrix4x4::RotationColumn(rotation) * Matrix4x4::Scale(scale); }
		inline Matrix4x4 GetWorldProjectionRow() const { return Matrix4x4::Translation(position) * Matrix4x4::Rotation(rotation) * Matrix4x4::Scale(scale); }
	};

	struct HELIOS_API Transform2D
	{
		Vector2	position	=  { 0.0f, 0.0f };
		Vector2 rotation	= { 0.0f, 0.0f };
		Size		size	= { 50.0f, 50.0f };
		
		Transform2D() = default;
		Transform2D(const Transform2D&) = default;
		Transform2D(const Vector2& position, const Size& size = { 50.0f, 50.0f }) : position(position), size(size) { }
	};

	struct HELIOS_API SpriteRenderer
	{
		Color color;
		Ref<Texture2D> texture;

		SpriteRenderer() = default;
		SpriteRenderer(const SpriteRenderer&) = default;
		SpriteRenderer(const Color& color) : color(color) { };
	};

	struct HELIOS_API MeshRenderer
	{
		Ref<Mesh> mesh;
		Ref<Material> material;
	
		MeshRenderer() = default;
		MeshRenderer(const MeshRenderer&) = default;
		MeshRenderer(const Ref<Mesh>& mesh) : mesh(mesh) { }
	};

	struct HELIOS_API Camera
	{
		float near_z = 0.5f;
		float far_z = 1000.0f;
		float size = 1.0f;
		float fov = 60.0f;
		Color clear_color = { 0.0f, 0.0f, 0.0f, 1.0f };
		bool ortographic = false;
		bool isPrimary = false;

		Camera() = default;
		Camera(const Camera&) = default;
		Camera(const Color& color) : clear_color(color) { };
	};
}
