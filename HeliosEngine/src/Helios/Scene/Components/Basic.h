#pragma once
#include "pch.h"
#include "Helios/Core/UUID.h"
#include "Helios/Resources/Color.h"
#include "Helios/Resources/Texture.h"

namespace Helios
{
	struct HELIOS_API GlobalObjectComponent { std::byte a; };
	struct HELIOS_API DontDestroyOnLoadComponent { std::byte a; };
	struct HELIOS_API DisabledObjectComponent { std::byte a; };

	struct HELIOS_API UUIDComponent
	{
		::Helios::UUID uuid;
		UUIDComponent() = default;
		UUIDComponent(const UUIDComponent&) = default;
		UUIDComponent(const UUID& uuid)
		{
			this->uuid = uuid;
		}
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
		enum class BackgroundMode
		{
			None,
			SolidColor,
			Skybox,
		};

		float near_z = 0.5f;
		float far_z = 1000.0f;
		float size = 1.0f;
		float fov = 60.0f;
		bool ortographic = false;
		bool isPrimary = false;

		BackgroundMode background_mode = BackgroundMode::SolidColor;
		Color clear_color = Color::Black;
		Ref<Texture2D> skybox_texture;

		CameraComponent() = default;
		CameraComponent(const CameraComponent&) = default;
		CameraComponent(const Color& color) : clear_color(color) { }
	};
}