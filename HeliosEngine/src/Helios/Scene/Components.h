#pragma once

#include "Helios/Core/Base.h"

#include "Helios/Translation/Vector.h"
#include "Helios/Translation/Quanterion.h"
#include "Helios/Resources/Color.h"
#include "Helios/Resources/Mesh.h"
#include "Helios/Resources/Texture.h"
#include "Helios/Resources/Material.h"

namespace Helios::Components {

	struct Relationship
	{
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
			}
			else {
				parent_handle = entt::null;
				prev_child = entt::null;
				next_child = entt::null;
			}
		}
	};

	struct HELIOS_API InfoComponent
	{
		std::string name;

		InfoComponent() = default;
		InfoComponent(const InfoComponent&) = default;
		InfoComponent(const std::string& name) : name(name) { }
	};

	struct HELIOS_API Transform
	{
		Vector3		position = Vector3::Zero();
		Quanterion	rotation = Quanterion::Identity();
		// Vector3	 rotationVec = { 0.0f, 0.0f, 0.0f };
		Vector3		scale	 = { 1.0f, 1.0f, 1.0f };
		bool typeSwitch = false;

		Transform() = default;
		Transform(const Transform&) = default;
		Transform(const Vector3& position) : position(position) { }
		Transform(const Vector3& position, const Vector3& rotation) : position(position), rotation(rotation) { }
		Transform(const Vector3& position, const Vector3& rotation, const Vector3& scale) : position(position), rotation(rotation), scale(scale) { }

		void Rotate(const Vector3& euler) {
			//rotation *= Quanterion::Euler(euler);
			rotation = Quanterion::FromEuler(rotation.euler() + euler);
		}
		void RotateRads(const Vector3& euler) { this->rotation = this->rotation * Quanterion::FromEulerRads(euler); }
		
		inline Vector3 forward() { return rotation.forward(); }
		//Vector3 forward() { return rotation * Vector3::Forward(); }
		inline Vector3 right() { return rotation * Vector3::Right(); }
	};

	struct HELIOS_API Transform2D
	{
		Vector2	position	= { 0.0f, 0.0f };
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

		Camera() = default;
		Camera(const Camera&) = default;
		Camera(const Color& color) : clear_color(color) { };
	};
}