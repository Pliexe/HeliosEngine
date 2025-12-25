#pragma once

#include "Event.h"

namespace Helios::Scripting
{
    class BehaviourEnableEvent : public BehaviourEvent
    {
    public:
        BehaviourEnableEvent(Behaviour& behaviour) : BehaviourEvent(behaviour) { }
        EVENT_TYPE(BehaviourEnable)
    };

    class BehaviourDisableEvent : public BehaviourEvent
    {
    public:
        BehaviourDisableEvent(Behaviour& behaviour) : BehaviourEvent(behaviour) { }
        EVENT_TYPE(BehaviourDisable)
    };
}