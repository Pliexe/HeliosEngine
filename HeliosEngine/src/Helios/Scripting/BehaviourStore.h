#pragma once

#include "pch.h"
#include "Behaviour.h"
#include "ScriptingService.h"
#include "Helios/Core/Base.h"
#include "BehaviourHandle.h"
#include "ScriptServiceEvent.h"
#include "Helios/Core/Exceptions.h"
#include "Helios/Scene/Entity.h"

#include "Helios/Events/BehaviourEvents.h"

namespace Helios::Scripting
{
    class HELIOS_API BehaviourStore
    {
    public:
        BehaviourStore() = delete;
        BehaviourStore(ScriptingService* service);
        // BehaviourStore(const BehaviourStore& other, WeakRef<EntityContainer> container);
        ~BehaviourStore();

        bool AddBehaviour(Entity entity, std::string_view behaviour_name, Helios::Scripting::Behaviour *pbehaviour = nullptr);
        bool AddBehaviour(Entity entity, Helios::Scripting::Behaviour *pbehaviour);
        bool RemoveBehaviour(const Behaviour& handle);

        bool HasBehaviour(Entity entity) const;
        std::pair<std::vector<Behaviour>::const_iterator, std::vector<Behaviour>::const_iterator> GetBehavioursIter(Entity entity) const;
        const std::vector<Behaviour> GetBehaviours(Entity entity) const;


        const std::vector<Behaviour>& GetBehaviours() const { return m_Behaviours; }
        void test();


        void SetEnabled(const Behaviour &behaviour, bool enabled);

        // BehaviourStore &operator =(const BehaviourStore&);

        // void OnAwake();
        // void OnStart();
        void OnUpdate();
        void OnEditorUpdate();

        void OnEvent(Event& event);

    private:
        friend Helios::Scripting::ServiceResult UnregisterService(std::string_view);
        friend Helios::Scripting::ServiceResult StartService(std::string_view);
        friend Helios::Scripting::ServiceResult StopService(std::string_view);
        friend Helios::Scripting::ServiceResult ReloadService(std::string_view);
        friend Helios::Scripting::ServiceResult ReloadService(std::string_view service_name, std::function<bool()> callback);

        void OnServiceStop();
        void OnServiceStart();

        std::vector<Behaviour> m_Behaviours = {};
        std::unordered_map<std::uintptr_t, size_t> m_BehaviourToIndex = {}; // Left it as uintptr_t for now in case ScriptHandle were to change for safety
        std::unordered_map<Entity, std::vector<size_t>> m_EntityToBehaviour = {};

        struct EntityPointerHasher {
        size_t operator()(Entity* entity) const {
                return std::hash<void*>{}(static_cast<void*>(entity));
            }
        };

        struct EntityPointerEquality {
            bool operator()(Entity* lhs, Entity* rhs) const {
                return lhs == rhs;
            }
        };

        std::unordered_set<Entity*, EntityPointerHasher, EntityPointerEquality> m_RefEntities = {};

        // std::unordered_map<Entity, std::vector<Behaviour>> m_Scripts2 = {};

        struct BatchCache
        {
            std::vector<BehaviourHandle> cache;
            bool dirty = false;

            void mark_dirty() { dirty = true; }
            void clear() { cache.clear(); dirty = false; }
        } m_updateCache;

        ScriptingService* m_service;

        // std::vector<Event> m_events;
    };
}