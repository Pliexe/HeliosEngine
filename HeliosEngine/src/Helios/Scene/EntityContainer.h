#pragma once
#include "pch.h"
#include "Helios/Core/Base.h"
#include "Helios/Core/UUID.h"
#include "Helios/Math/Vector.h"

namespace Helios
{
	class Entity;
	class HELIOS_API EntityContainer : public std::enable_shared_from_this<EntityContainer>
	{
	public:

		EntityContainer() : m_components(), m_UUIDMap() {}
		virtual ~EntityContainer() = default;

		inline void CloneFrom(EntityContainer& other);

		inline bool Contains(entt::entity entity);
		inline bool Contains(Entity entity);
		inline bool Contains(UUID uuid);

		inline Entity GetEntity(entt::entity entity);
		inline Entity GetEntity(UUID uuid);

		uint32_t GetEntityCount() { return m_components.size(); }

		static bool DestroyEntity(entt::entity entity);
		static bool DestroyEntity(UUID uuid);
		static bool DestroyEntity(Entity entity);

		// Clones the entity and returns the new entity (deep copy)
		Entity CloneEntity(Entity entity);
		// Copies the entity and returns the new entity (shallow copy)
		Entity CopyEntity(Entity entity);

		inline Entity CreateEntity();
		inline Entity CreateEntity(std::string name);
		inline Entity CreateEntity(entt::entity parent);
		inline Entity CreateEntity(Vector3 position);
		inline Entity CreateEntity(std::string name, entt::entity parent);
		Entity CreateEntity(std::string name, Vector3 position, entt::entity parent = entt::null);

		template <typename... T>
		auto GetComponentsWith()
		{
			return m_components.template view<T...>();
		}

		const entt::registry& GetComponents()	
		{
			return m_components;
		}

	protected:
		entt::registry m_components;
		std::unordered_map<UUID, entt::entity> m_UUIDMap;

		friend class Entity;
		friend class Transform;
	};
}
