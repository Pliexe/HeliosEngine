/* Copyright (c) 2022 Szabadi László Zsolt
 * You should have received a copy of the GNU AGPL v3.0 license with
 * this file. If not, please write to: pliexe, or visit : https://github.com/Pliexe/VisualDiscordBotCreator/blob/master/LICENSE
 */
#include "Entity.h"

#include "entt.hpp"

namespace Helios 
{	
	void Entity::SetParent(Entity& object)
	{
		//HL_CORE_ASSERT_WITH_MSG(!m_scene.expired(), "Cannot reset Parent while Scene is unloaded!");
		GetComponent<RelationshipComponent>().SetParent(m_scene->m_components, m_entityHandle, object);
	}

	void Entity::SetParent(entt::entity& object)
	{
		//HL_CORE_ASSERT_WITH_MSG(!m_scene.expired(), "Cannot reset Parent while Scene is unloaded!");
		GetComponent<RelationshipComponent>().SetParent(m_scene->m_components, m_entityHandle, object);
	}

	void Entity::ResetParent()
	{
		//HL_CORE_ASSERT_WITH_MSG(!m_scene.expired(), "Cannot reset Parent while Scene is unloaded!");
		GetComponent<RelationshipComponent>().Reset(m_scene->m_components);
	}

	//Transform2DComponent GameObject::relationship = Transform2DComponent();

	void Entity::MakeGlobal() { AddComponent<GlobalObjectComponent>(); }

	void Entity::Destroy()
	{
		//HL_CORE_ASSERT_WITH_MSG(!m_scene.expired(), "Can't destory GameObject if a scene is unloaded!");
		//auto scene = m_scene.lock();
		m_scene->m_components.destroy(m_entityHandle);
	}

	
}
