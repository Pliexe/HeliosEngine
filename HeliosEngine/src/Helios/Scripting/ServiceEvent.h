#pragma once

#ifndef SCRIPT_EVENT_H
#define SCRIPT_EVENT_H

#include "pch.h"

#define HELIOS_ENTITY_DESTROYED 0x0

#ifdef __cplusplus
namespace Helios
{
	enum ServiceEvent : std::uint8_t
	{
		OnEntityDestroyed = HELIOS_ENTITY_DESTROYED,
	};
}
#endif

#endif