#pragma once

#include "Helios/Core/Asserts.h"
#include "Helios/Core/Base.h"
#include "Helios/Scene/EntityContainer.h"

namespace Helios
{
    class HELIOS_API EntityHot
    {
        public:
        EntityHot() = default;
        EntityHot(const EntityHot &other) = default;
        EntityHot(entt::entity entity_id, EntityContainer* container)
        {
            m_container = container;
            m_entityHandle = entity_id;
        }
        
        template <typename... T>
		bool HasComponent()
		{
			HL_ASSERT(m_entityHandle != entt::null, "entity handle is invalid!");
			HL_ASSERT(m_container != nullptr, "Scene does not exist anymore!");
			return m_container->m_components.template any_of<T...>(m_entityHandle);
		}

		template <typename T, typename... Args>
		T &AddScopedComponent(Args &&...args)
		{
			HL_ASSERT(!HasComponent<T>(), "GameObject arleady has this component!");
			HL_ASSERT(m_container != nullptr, "Scene does not exist anymore!");
			T &comp = m_container->m_components.emplace<T>(m_entityHandle, std::forward<Args>(args)...);
			return comp;
		}

		template <typename T, typename... Args>
		T &AddComponent(Args &&...args)
		{
			T &comp = m_container->m_components.emplace<T>(m_entityHandle, std::forward<Args>(args)...);
			return comp;
		}

		template <typename T, typename... Args>
		T &AddOrReplaceComponent(Args &&...args)
		{
			T &comp = m_container->m_components.emplace_or_replace<T>(m_entityHandle, std::forward<Args>(args)...);
			return comp;
		}

		template <typename T>
		void RemoveComponent()
		{
			HL_ASSERT(HasComponent<T>(), "GameObject does not have the component!");
			HL_ASSERT(m_container != nullptr, "Scene does not exist anymore!");
			m_container->m_components.remove<T>(m_entityHandle);
		}

		template <typename... T>
		decltype(auto) GetComponent()
		{
			HL_ASSERT(HasComponent<T...>(), "GameObject does not have the component!");
			HL_ASSERT(m_container != nullptr, "Scene does not exist anymore!");
			return m_container->m_components.template get<T...>(m_entityHandle);
		}

        operator entt::entity() const { return m_entityHandle; }
		bool operator==(const EntityHot &other) const { return m_entityHandle == other.m_entityHandle && m_container == other.m_container; }

    private:
        EntityContainer* m_container;
        entt::entity m_entityHandle{entt::null};
    };
}
