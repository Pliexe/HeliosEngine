#pragma once

#ifndef SCRIPT_EVENT_H
#define SCRIPT_EVENT_H

#include "pch.h"

#define HELIOS_SCRIPT_EVENT_CREATE 0x0
#define HELIOS_SCRIPT_EVENT_DESTROY 0x1
#define HELIOS_SCRIPT_EVENT_AWAKE 0x2

#ifdef __cplusplus
namespace Helios
{
	enum ScriptEvent : std::uint8_t
	{
		OnCreate = HELIOS_SCRIPT_EVENT_CREATE,
		OnDestroy = HELIOS_SCRIPT_EVENT_DESTROY,
		OnAwake = HELIOS_SCRIPT_EVENT_AWAKE,
	};
}
#endif

#endif