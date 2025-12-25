#include "Behaviour.h"
#include "pch.h"
#include "Helios/Scene/Entity.h"

using namespace Helios;
using namespace Helios::Scripting;

Helios::Scripting::Behaviour::Behaviour(const Behaviour &other)
{
    m_Entity = other.m_Entity;
    m_BehaviourName = other.m_BehaviourName;
    m_ServiceName = other.m_ServiceName;
    m_Handle = other.m_Handle;
}

Behaviour::Behaviour(::Helios::Entity entity, const std::string &name, const std::string &serviceName) : m_BehaviourName(name), m_ServiceName(serviceName), m_Handle(0)
{
    m_Entity = CreateRef<Entity>(entity);
}
Behaviour::Behaviour(::Helios::Entity entity, std::string_view name, std::string_view serviceName) : m_BehaviourName(name), m_ServiceName(serviceName), m_Handle(0) {
    m_Entity = CreateRef<Entity>(entity);
}

Helios::Scripting::Behaviour::~Behaviour()
{
    // delete m_Entity;
}

Ref<Entity> Helios::Scripting::Behaviour::GetEntity() const
{
    return m_Entity;
}
