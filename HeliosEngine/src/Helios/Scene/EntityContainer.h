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

		EntityContainer();
		virtual ~EntityContainer();

		void CloneFrom(const Ref<EntityContainer>& other);

		bool Contains(entt::entity entity);
		bool Contains(Entity entity);
		bool Contains(UUID uuid);

		Entity GetEntity(entt::entity entity);
		Entity GetEntity(UUID uuid);

		constexpr size_t GetEntityCount() { return m_components.storage<entt::entity>().size(); }

		bool DestroyEntity(entt::entity entity);
		bool DestroyEntity(UUID uuid);
		bool DestroyEntity(Entity entity);

		// Clones the entity and returns the new entity (deep copy)
		Entity CloneEntity(Entity entity);
		// Copies the entity and returns the new entity (shallow copy)
		Entity CopyEntity(Entity entity);

		Entity CreateEntity();
		Entity CreateEntity(std::string name);
		Entity CreateEntity(entt::entity parent);
		Entity CreateEntity(Vector3 position);
		Entity CreateEntity(std::string name, entt::entity parent);
		Entity CreateEntity(std::string name, Vector3 position, entt::entity parent = entt::null);

		template <typename T>
		T& GetComponent(entt::entity entity)
		{
			return m_components.get<T>(entity);
		}

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
		friend class EntityHot;
		friend class Transform;
		friend class Scene;
	};
}
