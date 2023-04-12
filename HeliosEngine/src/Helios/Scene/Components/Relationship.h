#pragma once
#include "pch.h"

namespace Helios
{
	struct RelationshipComponent
	{
		entt::entity top_handle = entt::null;
		entt::entity parent_handle = entt::null;
		entt::entity first_child = entt::null;
		entt::entity prev_sibling = entt::null;
		entt::entity next_sibling = entt::null;

		RelationshipComponent() = default;
		RelationshipComponent(const RelationshipComponent&) = default;
		RelationshipComponent(entt::registry& registry, entt::entity destination, entt::entity source) { SetParent(registry, destination, source); }

		inline bool HasChild() { return first_child != entt::null; }
		inline bool HasParent() { return parent_handle != entt::null; }

		void SetParent(entt::registry& registry, entt::entity destination, entt::entity source)
		{
			Reset(registry);
			if (top_handle == entt::null) top_handle = source;
			parent_handle = source;
			auto& parent = registry.get<RelationshipComponent>(source);
			next_sibling = parent.first_child;
			parent.first_child = destination;
		}

		void Clear() {
			parent_handle = entt::null;
			first_child = entt::null;
			prev_sibling = entt::null;
			next_sibling = entt::null;
			top_handle = entt::null;
		}

		void Reset(entt::registry& registry) {
			if (parent_handle != entt::null) {
				if (prev_sibling == entt::null) {
					auto& parent = registry.get<RelationshipComponent>(parent_handle);
					parent.first_child = next_sibling;
					next_sibling = entt::null;
				}
				else {
					auto& prev = registry.get<RelationshipComponent>(prev_sibling);
					if (next_sibling != entt::null)
					{
						auto& next = registry.get<RelationshipComponent>(next_sibling);
						next.prev_sibling = prev_sibling;
						prev.next_sibling = next_sibling;
						next_sibling = entt::null;
						prev_sibling = entt::null;
					}
					else prev.next_sibling = entt::null;
				}
				parent_handle = entt::null;
				top_handle = entt::null;
			}
			else {
				parent_handle = entt::null;
				top_handle = entt::null;
				prev_sibling = entt::null;
				next_sibling = entt::null;
			}
		}
	};
}
