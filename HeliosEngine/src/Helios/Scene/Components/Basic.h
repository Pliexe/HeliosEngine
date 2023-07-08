#pragma once
#include "pch.h"
#include "Helios/Core/UUID.h"
#include "Helios/Resources/Color.h"

namespace Helios
{
	struct HELIOS_API GlobalObjectComponent { byte a; };
	struct HELIOS_API DontDestroyOnLoadComponent { byte a; };
	struct HELIOS_API DisabledObjectComponent { byte a; };

	struct HELIOS_API UUIDComponent
	{
		::Helios::UUID uuid;
		UUIDComponent() = default;
		UUIDComponent(const UUIDComponent&) = default;
		UUIDComponent(const UUID& uuid) : uuid(uuid) { }
	};

	struct HELIOS_API InfoComponent
	{
		std::string name;
		
		InfoComponent() = default;
		InfoComponent(const InfoComponent&) = default;
		InfoComponent(const std::string& name) : name(name) { }
	};

	struct HELIOS_API CameraComponent
	{
		float near_z = 0.5f;
		float far_z = 1000.0f;
		float size = 1.0f;
		float fov = 60.0f;
		Color clear_color = Color::Black;
		bool ortographic = false;
		bool isPrimary = false;


		CameraComponent() = default;
		CameraComponent(const CameraComponent&) = default;
		CameraComponent(const Color& color) : clear_color(color) { }
	};
}