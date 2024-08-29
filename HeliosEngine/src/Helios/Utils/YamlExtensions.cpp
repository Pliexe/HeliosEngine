#include "YamlExtensions.h"

YAML::Emitter& YAML::operator<<(YAML::Emitter& out, const Helios::Vector2& x)
{
	out << YAML::Flow;
	out << YAML::BeginSeq << x.x << x.y << YAML::EndSeq;
	return out;
}

YAML::Emitter& YAML::operator<<(YAML::Emitter& out, const Helios::Vector3& x)
{
	out << YAML::Flow;
	out << YAML::BeginSeq << x.x << x.y << x.z << YAML::EndSeq;
	return out;
}

YAML::Emitter& YAML::operator<<(YAML::Emitter& out, const Helios::Quaternion& x)
{
	out << YAML::Flow;
	out << YAML::BeginSeq << x.x << x.y << x.z << x.w << YAML::EndSeq;
	return out;
}

YAML::Emitter& YAML::operator<<(YAML::Emitter& out, const Helios::Color& x)
{
	out << YAML::Flow;
	out << YAML::BeginSeq << x.r << x.g << x.b << x.a << YAML::EndSeq;
	return out;
}

YAML::Emitter& YAML::operator<<(YAML::Emitter& out, const Helios::UUID& x)
{
	out << x.toString();
	return out;
}

YAML::Emitter& YAML::operator<<(YAML::Emitter& out, Helios::Entity entity)
{
	// Either return the UUID or 0 if the entity is invalid
	out << (entity.IsValid() ? entity.GetUUID().toString() : "0");
	return out;
}
