#pragma once

#include "pch.h"
#include "Helios/Core/Base.h"
#include "BehaviourHandle.h"

namespace Helios
{
    class Entity;
    namespace Scripting
    {
        struct HELIOS_API Behaviour
        {
        public:
            Behaviour() = delete;
            Behaviour(const Behaviour &other);
            Behaviour(Entity entity, const std::string &name, const std::string &serviceName);
            Behaviour(Entity entity, std::string_view name, std::string_view serviceName);
            ~Behaviour();

            inline const std::string &GetName() const { return m_BehaviourName; }
            inline const std::string &GetServiceName() const { return m_ServiceName; }
            inline const BehaviourHandle &GetHandle() const { return m_Handle; }
            Ref<Entity> GetEntity() const;
            inline bool IsEnabled() const { return m_Enabled; }

            bool operator==(const Behaviour &other) const { return m_Handle == other.m_Handle; }

        private:
            BehaviourHandle m_Handle;
            std::string m_BehaviourName;
            std::string m_ServiceName;
            bool m_Enabled = false;
            bool m_FirstStart = true;
            Ref<Entity> m_Entity;

            friend class BehaviourStore;
        };
    }
}