#pragma once

#include "Helios/Core/Base.h"

#include "Helios/Translation/Vector2D.h"
#include "Helios/Resources/Color.h"

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

	struct HELIOS_API Transform2D
	{
		Vector2D	position = { 0.0f,	0.0f };
		Size2D		size	 = { 50.0f, 50.0f };
		
		Transform2D() = default;
		Transform2D(const Transform2D&) = default;
		Transform2D(const Vector2D& position, const Size2D& size = { 50.0f, 50.0f }) : position(position), size(size) { }
	};

	struct HELIOS_API SpriteRenderer
	{
		Color color;

		SpriteRenderer() = default;
		SpriteRenderer(const SpriteRenderer&) = default;
		SpriteRenderer(const Color& color) : color(color) { };
	};

	struct HELIOS_API Camera
	{
		Color clear_color = { 255, 255, 255, 1.0f };

		Camera() = default;
		Camera(const Camera&) = default;
		Camera(const Color& color) : clear_color(color) { };
	};
}