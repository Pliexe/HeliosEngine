#include "EntityContainer.h"
#include "Entity.h"
#include "EntityHelpers.h"
#include "Helios/Scripting/BehaviourStore.h"
#include "Helios/Scripting/ScriptManager.h"
#include "Helios/Events/SceneEvents.h"

namespace Helios
{
	EntityContainer::EntityContainer(): m_components(), m_UUIDMap()
	{}

    EntityContainer::~EntityContainer()
    {
        std::cout << "DEALLOC" << std::endl;
        SceneDestroyedEvent event;
        Scripting::OnEvent(event);
    }

    void EntityContainer::CloneFrom(const Ref<EntityContainer> &other)
    {
  //      auto view = other->m_components.view<entt::entity>();
  //      //auto view = other->m_components.view<InfoComponent, TransformComponent, RelationshipComponent, UUIDComponent>();
		//for (auto entity : view)
		//{
		//	CloneEntity(other->GetEntity(entity));
		//}
		// idk if its slow, maybe i should it all at once but whatever

		m_components.clear();

        for (const auto view = other->m_components.view<entt::entity>(); auto & entity : view)
        {
            auto new_entity = Entity(m_components.create(entity), shared_from_this());
			CopyComponentIfExists(AllComponents{}, new_entity, other->GetEntity(entity));
	        if (new_entity.HasComponent<BoxCollider2D>()) new_entity.GetComponent<BoxCollider2D>().body = nullptr;
	        if (new_entity.HasComponent<CircleCollider2D>()) new_entity.GetComponent<CircleCollider2D>().body = nullptr;
        }

        for (auto& service_name : Scripting::GetRegisteredServices())
        {
            auto& behaviour_store = Scripting::GetBehaviourStore(service_name);
            auto& behaviours = behaviour_store.GetBehaviours();

            for (size_t i = 0, n = behaviours.size(); i < n; ++i) // iterate through all behaviours in the store until the new added behaviours (n = original size)
            {
                if (behaviours[i].GetEntity()->GetContainer().expired()) continue;

                if (behaviours[i].GetEntity()->GetContainer().lock() == other)
                {
                    GetEntity(behaviours[i].GetEntity()->m_entityHandle).AddBehaviour(service_name, behaviours[i].GetName(), nullptr);
                }
            }
            // for (auto& behaviour : behaviours)
            // {
            //     if (behaviour.GetEntity().GetContainer().expired()) continue;

            //     if (behaviour.GetEntity().GetContainer().lock() == other)
            //     {
            //         GetEntity(behaviour.GetEntity().m_entityHandle).AddBehaviour(service_name, behaviour.GetName(), nullptr);

            //         // ScriptManager::CreateScript(new_entity, behaviour.m_Language, behaviour.GetName(), &behaviour.m_Handle);
            //     }
            // }
        }
	}
    bool EntityContainer::Contains(entt::entity entity)
    {
        return m_components.valid(entity);
    }
    bool EntityContainer::Contains(Entity entity)
    {
		return m_components.valid(static_cast<entt::entity>(entity));
    }
    bool EntityContainer::Contains(UUID uuid)
    {
		return m_UUIDMap.find(uuid) != m_UUIDMap.end();
    }
    Entity EntityContainer::GetEntity(entt::entity entity)
    {
		return Entity(entity, shared_from_this());
    }
    Entity EntityContainer::GetEntity(UUID uuid)
    {
        return Entity(m_UUIDMap[uuid], shared_from_this());
    }
    bool EntityContainer::DestroyEntity(entt::entity entity)
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

		/*entity.m_container.lock()->m_components.v.visit(entity.m_entityHandle, [&](auto type, auto& component)
		{
			newEntity.m_container.lock()->m_components.emplace_or_replace(type, newEntity.m_entityHandle, component);
		});*/

        return newEntity;
    }
    Entity EntityContainer::CreateEntity() { return CreateEntity(std::string("Entity (") + std::to_string(GetEntityCount() + 1) + ")", Vector3(), entt::null); }
    Entity EntityContainer::CreateEntity(std::string name) { return CreateEntity(name, Vector3(), entt::null); }
	Entity EntityContainer::CreateEntity(entt::entity parent) { return CreateEntity(std::string("Entity (") + std::to_string(GetEntityCount() + 1) + ")", Vector3(), parent); }
	Entity EntityContainer::CreateEntity(Vector3 position) { return CreateEntity(std::string("Entity (") + std::to_string(GetEntityCount() + 1) + ")", position, entt::null); }
    Entity EntityContainer::CreateEntity(std::string name, entt::entity parent) { return CreateEntity(name, Vector3(), parent); }

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
