#include "EntityContainer.h"
#include "Entity.h"
#include "EntityHelpers.h"

namespace Helios
{
    inline void EntityContainer::CloneFrom(EntityContainer& other)
    {
		auto view = other.m_components.view<InfoComponent, TransformComponent, RelationshipComponent, UUIDComponent>();
		for (auto entity : view)
		{
            CloneEntity(other.GetEntity(entity));
		}
        // idk if its slow, maybe i should it all at once but whatever
    }
    inline bool EntityContainer::Contains(entt::entity entity)
    {
        return m_components.valid(entity);
    }
    inline bool EntityContainer::Contains(Entity entity)
    {
		return m_components.valid(static_cast<entt::entity>(entity));
    }
    inline bool EntityContainer::Contains(UUID uuid)
    {
		return m_UUIDMap.find(uuid) != m_UUIDMap.end();
    }
    inline Entity EntityContainer::GetEntity(entt::entity entity)
    {
		return Entity(entity, shared_from_this());
    }
    inline Entity EntityContainer::GetEntity(UUID uuid)
    {
        return Entity(m_UUIDMap[uuid], shared_from_this());
    }
    inline bool EntityContainer::DestroyEntity(entt::entity entity)
    {
        return false;
    }
    bool EntityContainer::DestroyEntity(UUID uuid)
    {
        return false;
    }
    bool EntityContainer::DestroyEntity(Entity entity)
    {
        return false;
    }
    Entity EntityContainer::CloneEntity(Entity entity)
    {
		HL_ASSERT(entity.IsValid(), "Entity is invalid!");
		Entity newEntity = CopyEntity(entity);
		if (newEntity.HasComponent<BoxCollider2D>()) newEntity.GetComponent<BoxCollider2D>().body = nullptr;
		if (newEntity.HasComponent<CircleCollider2D>()) newEntity.GetComponent<CircleCollider2D>().body = nullptr;
		return newEntity;
    }
    Entity EntityContainer::CopyEntity(Entity entity)
    {
        HL_ASSERT(entity.IsValid(), "Entity is invalid!");
        Entity newEntity = CreateEntity(entity.GetName() + " (clone)");
        CopyComponentIfExists(AllComponents{}, newEntity, entity);
        return newEntity;
    }
    inline Entity EntityContainer::CreateEntity() { return CreateEntity(std::string("Entity (") + std::to_string(m_components.size() + 1) + ")", Vector3(), entt::null); }
    inline Entity EntityContainer::CreateEntity(std::string name) { return CreateEntity(name, Vector3(), entt::null); }
	inline Entity EntityContainer::CreateEntity(entt::entity parent) { return CreateEntity(std::string("Entity (") + std::to_string(m_components.size() + 1) + ")", Vector3(), parent); }
	inline Entity EntityContainer::CreateEntity(Vector3 position) { return CreateEntity(std::string("Entity (") + std::to_string(m_components.size() + 1) + ")", position, entt::null); }
    inline Entity EntityContainer::CreateEntity(std::string name, entt::entity parent) { return CreateEntity(name, Vector3(), parent); }
    Entity EntityContainer::CreateEntity(std::string name, Vector3 position, entt::entity parent)
    {
		Entity newEntity(m_components.create(), shared_from_this());
        newEntity.AddScopedComponent<InfoComponent>(name);
		newEntity.AddScopedComponent<TransformComponent>(position);
		if (parent != entt::null)
		{
			newEntity.AddScopedComponent<RelationshipComponent>(m_components, newEntity, parent);
		}
		else
		{
			newEntity.AddOrReplaceComponent<RelationshipComponent>();
		}
		m_UUIDMap[newEntity.AddScopedComponent<UUIDComponent>().uuid] = newEntity;
		return newEntity;
    }
}
