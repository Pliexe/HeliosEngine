#pragma once

#include "pch.h"
#include "Helios/Core/UUID.h"
#include "Helios/Math/Vector.h"
#include "Helios/Math/Quaternion.h"
#include "Helios/Resources/Color.h"
#include "Helios/Scene/Entity.h"

namespace YAML
{
	template<>
	struct convert<Helios::Vector2>
	{
		static Node encode(const Helios::Vector2& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.SetStyle(YAML::EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, Helios::Vector2& rhs)
		{
			if (!node.IsSequence() || node.size() != 2)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			return true;
		}
	};

	template<>
	struct convert<Helios::Vector3>
	{
		static Node encode(const Helios::Vector3& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.SetStyle(YAML::EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, Helios::Vector3& rhs)
		{
			if (!node.IsSequence() || node.size() != 3)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			return true;
		}
	};

	template<>
	struct convert<Helios::Quaternion>
	{
		static Node encode(const Helios::Quaternion& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.push_back(rhs.w);
			node.SetStyle(YAML::EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, Helios::Quaternion& rhs)
		{
			if (!node.IsSequence() || node.size() != 4)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			rhs.w = node[3].as<float>();
			return true;
		}
	};

	template<>
	struct convert<Helios::Color>
	{
		static Node encode(const Helios::Color& rhs)
		{
			Node node;
			node.push_back(rhs.r);
			node.push_back(rhs.g);
			node.push_back(rhs.b);
			node.push_back(rhs.a);
			node.SetStyle(YAML::EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, Helios::Color& rhs)
		{
			if (!node.IsSequence() || node.size() != 4)
				return false;

			rhs.r = node[0].as<float>();
			rhs.g = node[1].as<float>();
			rhs.b = node[2].as<float>();
			rhs.a = node[3].as<float>();
			return true;
		}
	};

	template<>
	struct convert<Helios::UUID>
	{
		static Node encode(const Helios::UUID& rhs)
		{
			Node node;
			node = rhs.toString();
			return node;
		}

		static bool decode(const Node& node, Helios::UUID& rhs)
		{
			if (!node.IsScalar())
				return false;

			rhs = Helios::UUID::fromString(node.as<std::string>());
			return true;
		}
	};

	YAML::Emitter& operator<<(YAML::Emitter& out, const Helios::Vector2& x);

	YAML::Emitter& operator<<(YAML::Emitter& out, const Helios::Vector3& x);

	YAML::Emitter& operator<<(YAML::Emitter& out, const Helios::Quaternion& x);

	YAML::Emitter& operator<<(YAML::Emitter& out, const Helios::Color& x);

	YAML::Emitter& operator<<(YAML::Emitter& out, const Helios::UUID& x);

	YAML::Emitter& operator<<(YAML::Emitter& out, Helios::Entity entity);
}