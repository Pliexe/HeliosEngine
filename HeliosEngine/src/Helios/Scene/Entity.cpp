/* Copyright (c) 2022 Szabadi L�szl� Zsolt
 * You should have received a copy of the GNU AGPL v3.0 license with
 * this file. If not, please write to: pliexe, or visit : https://github.com/Pliexe/HeliosEngine/blob/master/HeliosEngine/LICENSE.txt
 */
#include "Entity.h"

#include "Helios/Scripting/BehaviourStore.h"
#include "Helios/Scripting/ScriptManager.h"

#include "entt.hpp"

namespace Helios
{
	void Entity::SetParent(Entity &object)
	{
		// HL_ASSERT(!m_scene.expired(), "Cannot reset Parent while Scene is unloaded!");
		GetComponent<RelationshipComponent>().SetParent(m_container.lock()->m_components, m_entityHandle, object);
	}

	void Entity::SetParent(entt::entity &object)
	{
		// HL_ASSERT(!m_scene.expired(), "Cannot reset Parent while Scene is unloaded!");
		GetComponent<RelationshipComponent>().SetParent(m_container.lock()->m_components, m_entityHandle, object);
	}

	void Entity::ResetParent()
	{
		// HL_ASSERT(!m_scene.expired(), "Cannot reset Parent while Scene is unloaded!");
		GetComponent<RelationshipComponent>().Reset(m_container.lock()->m_components);
	}

	// Transform2DComponent GameObject::relationship = Transform2DComponent();

	void Entity::MakeGlobal() { AddScopedComponent<GlobalObjectComponent>(); }

	void Entity::Destroy()
	{
		// HL_ASSERT(!m_scene.expired(), "Can't destory GameObject if a scene is unloaded!");
		// auto scene = m_scene.lock();
		m_container.lock()->m_components.destroy(m_entityHandle);
	}

	bool Entity::HasBehaviours(std::string_view service_name)
	{ return m_container.expired() ? false : Scripting::GetBehaviourStore(service_name).HasBehaviour(*this); }

	bool Entity::AddBehaviour(std::string_view service_name, std::string_view behaviour_name, Scripting::Behaviour* outHandle)
	{
		if (m_container.expired())
			return false;

		return Scripting::GetBehaviourStore(service_name).AddBehaviour(*this, behaviour_name, nullptr); // m_container.lock()->GetBehaviourStore()->AddBehaviour(*this, language, name, outHandle);
	}

	bool Entity::RemoveBehaviour(std::string_view service_name, const Scripting::Behaviour& handle)
	{
		if (m_container.expired())
			return false;
				
		return Scripting::GetBehaviourStore(service_name).RemoveBehaviour(handle);
	}

	std::pair<std::vector<Scripting::Behaviour>::const_iterator, std::vector<Scripting::Behaviour>::const_iterator> Entity::GetBehavioursIter(std::string_view service_name)
	{ 
		static const std::vector<Scripting::Behaviour> empty;
		static const std::pair<std::vector<Scripting::Behaviour>::const_iterator, std::vector<Scripting::Behaviour>::const_iterator> empty_pair {empty.begin(), empty.end()};
		return m_container.expired() ? empty_pair : Scripting::GetBehaviourStore(service_name).GetBehavioursIter(*this);
	}

    const std::vector<Scripting::Behaviour> Entity::GetBehaviours(std::string_view service_name) const
    {
		static const std::vector<Scripting::Behaviour> empty;
		return m_container.expired() ? empty : Scripting::GetBehaviourStore(service_name).GetBehaviours(*this);
    }

    // const std::vector<Scripting::Behaviour>& Entity::GetBehaviours(std::string_view service_name)
	// { 
	// 	static const std::vector<Scripting::Behaviour> empty;
	// 	return m_container.expired() ? empty : Scripting::GetBehaviourStore(service_name).GetBehavioursIter(*this);
	// }
}

extern "C"
{
	using Helios::Transform;
	HELIOS_API Transform* TransformHandleClone(Transform *transform) { return new Transform(*transform); }
	// HELIOS_API 
	HELIOS_API void TransformHandleDestroy(Transform *transform) { delete transform; }
}

extern "C"
{
	using Helios::Entity;
	HELIOS_API bool EntityIsValid(Entity *entity)
	{
		HL_ASSERT(entity != nullptr, "Entity is null!");
		HL_ASSERT(dynamic_cast<Helios::Entity *>(entity) != nullptr, "Entity is not an instance of Helios::Entity!");
		return entity->IsValid();
	}
	HELIOS_API bool EntityIsEqual(Entity *a, Entity *b)
	{
		HL_ASSERT(a != nullptr, "Entity is null!");
		HL_ASSERT(dynamic_cast<Helios::Entity *>(a) != nullptr, "Entity is not an instance of Helios::Entity!");
		HL_ASSERT(b != nullptr, "Entity is null!");
		HL_ASSERT(dynamic_cast<Helios::Entity *>(b) != nullptr, "Entity is not an instance of Helios::Entity!");
		return *a == *b;
	}
	HELIOS_API bool EntityIsNull(Entity *entity)
	{
		HL_ASSERT(entity != nullptr, "Entity is null!");
		HL_ASSERT(dynamic_cast<Helios::Entity *>(entity) != nullptr, "Entity is not an instance of Helios::Entity!");
		return entity->IsNull();
	}


	HELIOS_API Entity *EntityHandleClone(Entity *entity)
	{
		HL_ASSERT(entity != nullptr, "Entity is null!");
		HL_ASSERT(dynamic_cast<Helios::Entity *>(entity) != nullptr, "Entity is not an instance of Helios::Entity!");
		return new Entity(*entity);
	}
	HELIOS_API void EntityDestroy(Entity *entity)
	{
		HL_ASSERT(entity != nullptr, "Entity is null!");
		HL_ASSERT(dynamic_cast<Helios::Entity *>(entity) != nullptr, "Entity is not an instance of Helios::Entity!");
		entity->Destroy();
	}

	HELIOS_API void EntityHandleDestroy(Entity *entity)
	{
		HL_ASSERT(entity != nullptr, "Entity is null!");
		HL_ASSERT(dynamic_cast<Helios::Entity *>(entity) != nullptr, "Entity is not an instance of Helios::Entity!");
		delete entity;
	}

	HELIOS_API uint32_t EntityGetName(Entity *entity, char* buffer, uint32_t buffer_size)
	{
		if (entity == nullptr) return 0;
		HL_ASSERT(dynamic_cast<Helios::Entity *>(entity) != nullptr, "Entity is not an instance of Helios::Entity!");

		uint32_t len = static_cast<uint32_t>(entity->GetName().size()) + 1u;

		if (buffer == nullptr || buffer_size == 0) return len;

		if (len > buffer_size) return 0;

		std::strncpy(buffer, entity->GetName().c_str(), buffer_size - 1);
		buffer[buffer_size - 1] = '\0';

		return len;
	}

	HELIOS_API bool SetEntityName(Entity *entity, const char* name)
	{
		if (entity == nullptr) return false;
		if (name == nullptr) return false;
		HL_ASSERT(dynamic_cast<Helios::Entity *>(entity) != nullptr, "Entity is not an instance of Helios::Entity!");
		entity->GetComponent<Helios::InfoComponent>().name = name;
		return true;
	}
}