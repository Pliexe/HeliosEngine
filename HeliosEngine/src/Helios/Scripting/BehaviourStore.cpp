#include "BehaviourStore.h"
#include "Helios/Core/Base.h"
#include "Helios/Core/Exceptions.h"
#include "Helios/Events/SceneEvents.h"

using namespace Helios::Scripting;

BehaviourStore::BehaviourStore(ScriptingService *service) : m_service(service)
// : m_container(container)
{
}


BehaviourStore::~BehaviourStore()
{
}

bool BehaviourStore::AddBehaviour(Entity entity, std::string_view behaviour_name, Behaviour *pbehaviour)
{
    if (!entity.IsValid())
        return false;
    if (pbehaviour != nullptr)
        return AddBehaviour(entity, pbehaviour);
    else
    {
        Behaviour behaviour = Behaviour(entity, behaviour_name, m_service->GetName());
        return AddBehaviour(entity, &behaviour);
    }
}

bool BehaviourStore::AddBehaviour(Entity entity, Behaviour *pbehaviour)
{
    m_updateCache.mark_dirty();

    Entity* entityPtr = &entity;
    auto it = m_RefEntities.find(entityPtr);
    if (it == m_RefEntities.end()) {
        entityPtr = new Entity(entity);
        m_RefEntities.insert(entityPtr);
    }

    if (m_service->CreateBehaviour(pbehaviour->GetName(), (std::uintptr_t)(entityPtr), &pbehaviour->m_Handle) == Scripting::ServiceResult::Ok)
    {
        size_t index = m_Behaviours.size();

        m_Behaviours.reserve(index + 1); // since index is size
        m_Behaviours.push_back(*pbehaviour);

        m_BehaviourToIndex[pbehaviour->m_Handle] = index;
        m_EntityToBehaviour[entity].reserve(m_EntityToBehaviour[entity].size() + 1);
        m_EntityToBehaviour[entity].push_back(index);
        return true;
    }
    return false;
}

bool BehaviourStore::RemoveBehaviour(const Behaviour &behaviour)
{
    auto handle = behaviour.m_Handle; // MUST MAKE COPY TO AVOID HAVINGHANDLE CHANGE TO NEW ONE IN MIDDLE OF REMOVE
    auto it = m_BehaviourToIndex.find(handle);
    if (it == m_BehaviourToIndex.end())
        return false;

    m_updateCache.mark_dirty();

    size_t index = it->second;
    Behaviour& last = m_Behaviours.back();

    m_Behaviours[index] = last;
    m_BehaviourToIndex[last.m_Handle] = index;

    auto lastEntityPtr = last.GetEntity();
    if (lastEntityPtr) {
        Entity lastEntity = *lastEntityPtr;
        auto entityIt = m_EntityToBehaviour.find(lastEntity);
        if (entityIt != m_EntityToBehaviour.end())
        {
            auto& indices = entityIt->second;
            indices.erase(std::remove(indices.begin(), indices.end(), m_Behaviours.size() - 1), indices.end());
            indices.push_back(index);
        }
    } else HL_EXCEPTION(true, "Behaviour has no entity");

    m_Behaviours.pop_back();
    m_BehaviourToIndex.erase(handle);

    // TODO: Send event

    return m_service->DestroyBehaviour(handle) == Scripting::ServiceResult::Ok;
}

bool BehaviourStore::HasBehaviour(Entity entity) const
{
    return m_EntityToBehaviour.find(entity) != m_EntityToBehaviour.end();
}

std::pair<std::vector<Behaviour>::const_iterator, std::vector<Behaviour>::const_iterator> BehaviourStore::GetBehavioursIter(Entity entity) const
{
    const auto indices = m_EntityToBehaviour.at(entity);

    if (indices.empty()) {
        return {m_Behaviours.end(), m_Behaviours.end()};
    }

    auto startIter = m_Behaviours.begin() + indices.front();
    auto endIter = m_Behaviours.begin() + indices.back() + 1; // +1 for inclusive range if needed

    return {startIter, endIter};
}

const std::vector<Helios::Scripting::Behaviour> Helios::Scripting::BehaviourStore::GetBehaviours(Entity entity) const
{
    const auto& indices = m_EntityToBehaviour.at(entity);
    // return std::span<const Behaviour>(m_Behaviours.data() + indices.front(), indices.size());
    std::vector<Behaviour> behavioursForEntity;
    behavioursForEntity.reserve(indices.size());
    for (auto index : indices)
    {
        behavioursForEntity.push_back(m_Behaviours[index]);
    }
    return behavioursForEntity;
}

void Helios::Scripting::BehaviourStore::test()
{
    std::cout << "test start" << "\n";
    for (auto& behToIndex : m_BehaviourToIndex)
    {
        std::cout << "(" << behToIndex.first << ", " << (uint32_t)behToIndex.second << ")" << "\n";
    }
    std::cout << "test end" << "\n";
}

void Helios::Scripting::BehaviourStore::SetEnabled(const Behaviour &behaviour, bool enabled)
{
    m_updateCache.mark_dirty();

    auto it = m_BehaviourToIndex.find(behaviour.m_Handle);
    if (it == m_BehaviourToIndex.end())
        return;

    size_t index = it->second;
    m_Behaviours[index].m_Enabled = enabled;
}

// BehaviourStore &Helios::BehaviourStore::operator=(const BehaviourStore &other)
// {
//     m_Scripts = other.m_Scripts;

//     for (auto &[entity, scripts] : m_Scripts)
//     {
//         for (auto &[language, behaviours] : scripts)
//         {
//             for (auto &behaviour : behaviours)
//             {
//                 ScriptManager::CreateScript(m_container.lock()->GetEntity(entity), behaviour.m_Language, behaviour.GetName(), &behaviour.m_Handle);
//             }
//         }
//     }

//     return *this;
// }
void BehaviourStore::OnUpdate()
{
    if (m_updateCache.dirty)
    {
        m_updateCache.cache.clear();

        for (auto& behaviour : m_Behaviours)
        {
            if (behaviour.m_Handle == 0)
                continue;
            if (!behaviour.m_Enabled)
                continue;

            if (behaviour.m_Entity->HasComponent<DisabledObjectComponent>())
                continue;

            m_updateCache.cache.push_back(behaviour.m_Handle);
        }
    }

    for (auto &x : m_updateCache.cache)
    {
        m_service->CallBehaviourMethod(x, "Update");
    }

    // for (auto &x : m_events)
    // {
    //     EventDispatcher dispatcher(x);
    //     dispatcher.Dispatch<BehaviourEnableEvent>([&](BehaviourEvent &event) { m_service->CallBehaviourMethod(event.GetBehaviour().m_Handle, "OnEnable"); return true; });
    //     dispatcher.Dispatch<BehaviourDisableEvent>([&](BehaviourEvent &event) { m_service->CallBehaviourMethod(event.GetBehaviour().m_Handle, "OnDisable"); return true; });
    // }
}

void Helios::Scripting::BehaviourStore::OnEditorUpdate()
{
    // m_events.clear();
}

void Helios::Scripting::BehaviourStore::OnEvent(Event &event)
{
    EventDispatcher dispatcher(event);
    dispatcher.Dispatch<SceneDestroyedEvent>([this](SceneDestroyedEvent &e) {
        m_updateCache.mark_dirty();
        std::cout << "SceneDestroyedEvent" << std::endl;
        for (auto it = m_Behaviours.begin(); it != m_Behaviours.end(); )
        {
            std::cout << "Entity: " << (uint32_t)it->GetEntity()->GetHandle() << ", Container: " << it->GetEntity()->GetContainer().use_count() << ", Expired: " << it->GetEntity()->GetContainer().expired() << std::endl;
            if (it->GetEntity()->GetContainer().expired()) {
                Behaviour& behaviour = *it;
                this->RemoveBehaviour(behaviour);
            }
            else {
                ++it;
            }
        }
        return true;
    });
}

void BehaviourStore::OnServiceStop()
{
    m_updateCache.clear();

    for (auto& behaviour : m_Behaviours)
    {
        HL_ASSERT(behaviour.m_Handle != 0, "Behaviour handle is null");
        m_service->DestroyBehaviour(behaviour.m_Handle);
        behaviour.m_Handle = 0;
    }

    m_BehaviourToIndex.clear();
}
void BehaviourStore::OnServiceStart()
{
    m_updateCache.mark_dirty();

    for (size_t i = 0; i < m_Behaviours.size(); ++i)
    {
        HL_ASSERT(m_Behaviours[i].m_Handle == 0, "Behaviour handle is not null");
        m_service->CreateBehaviour(m_Behaviours[i].GetName(), (std::uintptr_t)m_Behaviours[i].m_Entity.get(), &m_Behaviours[i].m_Handle);
        m_BehaviourToIndex[m_Behaviours[i].m_Handle] = i;

        if (m_Behaviours[i].m_Enabled)
        {
            m_service->CallBehaviourMethod(m_Behaviours[i].m_Handle, "Start");
            m_Behaviours[i].m_FirstStart = false;
        }
    }
}
